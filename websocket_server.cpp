#include "websocket_server.hpp"

#include <iostream>
#include <functional>

#include <boost/filesystem.hpp>

#include <rapidjson/error/en.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <valijson/adapters/rapidjson_adapter.hpp>
#include <valijson/utils/file_utils.hpp>
#include <valijson/schema.hpp>
#include <valijson/schema_parser.hpp>

#include "jeopardy_exception.hpp"
#include "invalid_json.hpp"

using namespace std;
using namespace std::placeholders;
using namespace boost::filesystem;
using namespace rapidjson;
using namespace websocketpp;

websocket_server::websocket_server()
{
    this->init_asio();
    this->set_open_handler(bind(&websocket_server::on_connection_open, this, placeholders::_1));
    this->set_message_handler(bind(&websocket_server::on_message, this, placeholders::_1, placeholders::_2));

    // Initialize json validators
    path schema_dir = path("json-schema") / "events";
    if (!is_directory(schema_dir)) {
        throw jeopardy_exception("'json-schema/events' is not a directory");
    }
    for (directory_iterator it(schema_dir), end;it != end;it++)
    {
        path file = *it;
        if (!is_regular_file(file))
        {
            cerr << "Warning: " << file.string() << " is not a regular file" << endl;
            continue;
        }
        Document schema_doc;
        string json;
        valijson::utils::loadFile(file.string(), json);
        schema_doc.Parse(json.c_str());
        if (schema_doc.HasParseError())
        {
            cerr << "Warning: Error while parsing " + file.string() + ": " + GetParseError_En(schema_doc.GetParseError()) << endl;
            continue;
        }

        valijson::Schema schema;
        valijson::SchemaParser parser;
        valijson::adapters::RapidJsonAdapter schemaAdapter(schema_doc);
        parser.populateSchema(schemaAdapter, schema);
        validators.emplace(file.stem().string(), unique_ptr<valijson::Validator>(new valijson::Validator(schema)));
    }
}

void websocket_server::start_listen(int port)
{
    this->set_reuse_addr(true);
    this->listen(port);
    boost::system::error_code ec;
    this->start_accept();
}

void websocket_server::on_message(connection_hdl hdl, message_ptr message)
{
    try
    {
        Document d;
        d.Parse(message->get_payload().c_str());
        if (d.HasParseError())
            throw invalid_json(valijson::ValidationResults::Error({message->get_payload()}, GetParseError_En(d.GetParseError())));

        valijson::adapters::RapidJsonAdapter targetAdapter(d);
        valijson::ValidationResults results;

        if (!validators["basic"]->validate(targetAdapter, &results))
            throw invalid_json(results);

        string event = d["event"].GetString();
        try
        {
            if (!validators.at(event)->validate(targetAdapter, &results))
                throw invalid_json(valijson::ValidationResults::Error({message->get_payload()}, GetParseError_En(d.GetParseError())));
        }
        catch (out_of_range &)
        {
            throw invalid_json(valijson::ValidationResults::Error({message->get_payload()}, "Unknown event '" + event + "'"));
        }
        if (event == "ready") {
            connection_open.raise(hdl);
        }
        client_event.raise(d);
    }
    catch (invalid_json &e)
    {
        Document d;
        d.SetObject();
        d.AddMember("error", "invalid_json", d.GetAllocator());
        Value errors;
        errors.SetArray();
        for (auto &error : e.get_errors())
        {
            Value errValue;
            errValue.SetObject();
            errValue.AddMember("description", Value(error.description.c_str(), error.description.size()), d.GetAllocator());
            Value contextValue;
            contextValue.SetArray();
            for (auto &context : error.context)
            {
                contextValue.PushBack(Value(context.c_str(), context.size()), d.GetAllocator());
            }
            errValue.AddMember("context", contextValue, d.GetAllocator());
            errors.PushBack(errValue, d.GetAllocator());
        }
        d.AddMember("errors", errors, d.GetAllocator());
        this->broadcast(d);
    }
    catch (jeopardy_exception &e)
    {
        Document d;
        d.SetObject();
        d.AddMember("error", "jeopardy_exception", d.GetAllocator());
        d.AddMember("message", Value(e.what(), string(e.what()).size()), d.GetAllocator());
        this->broadcast(d);
    }
    catch (std::exception &e)
    {
        Document d;
        d.SetObject();
        d.AddMember("error", "exception", d.GetAllocator());
        d.AddMember("message", Value(e.what(), string(e.what()).size()), d.GetAllocator());
        this->broadcast(d);
    }
}

void websocket_server::on_connection_open(connection_hdl hdl)
{
    connections.insert(hdl);
}

void websocket_server::on_connection_close(connection_hdl hdl)
{
    // TODO Raise event
    connections.erase(hdl);
}

void websocket_server::send_document(websocketpp::connection_hdl hdl, const rapidjson::Document &d)
{
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    d.Accept(writer);
    string json = buffer.GetString();
    this->send(hdl, json, frame::opcode::value::TEXT);
}

void websocket_server::broadcast(const Document &data)
{
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    data.Accept(writer);
    string json = buffer.GetString();
    for (const connection_hdl &hdl : connections)
    {
        this->send(hdl, json, frame::opcode::value::TEXT);
    }
}

void websocket_server::shutdown()
{
    // TODO Call in destructor
    this->stop_listening();
    for (const connection_hdl &hdl : connections)
    {
        this->close(hdl, 0, "Server shutting down");
    }
    this->stop();
}
