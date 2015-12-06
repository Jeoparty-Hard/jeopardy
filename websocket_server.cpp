#include "websocket_server.hpp"

#include <iostream>
#include <functional>

#include <boost/filesystem.hpp>

#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <valijson/adapters/rapidjson_adapter.hpp>
#include <valijson/utils/file_utils.hpp>
#include <valijson/schema.hpp>
#include <valijson/schema_parser.hpp>

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
    if (!is_directory(schema_dir))
        throw 0; // TODO Handle error properly
    for (directory_iterator it(schema_dir), end;it != end;it++)
    {
        path file = *it;
        if (!is_regular_file(file))
            continue; // TODO Warning?
        Document schema_doc;
        string json;
        valijson::utils::loadFile(file.string(), json);
        schema_doc.Parse(json.c_str()); // TODO Check if schema is valid JSON file

        valijson::Schema schema;
        valijson::SchemaParser parser;
        valijson::adapters::RapidJsonAdapter schemaAdapter(schema_doc);
        parser.populateSchema(schemaAdapter, schema);
        validators.emplace(file.stem().string(), unique_ptr<valijson::Validator>(new valijson::Validator(schema)));

    }
}

void websocket_server::start_listen(int port)
{
    // TODO Reuse address
    this->listen(port);
    this->start_accept();
}

void websocket_server::on_message(connection_hdl hdl, message_ptr message)
{
    Document d;
    d.Parse(message->get_payload().c_str()); // TODO Check if file is valid json
    valijson::adapters::RapidJsonAdapter targetAdapter(d);
    valijson::ValidationResults results;
    cout << "starting validation" << endl;
    for (auto &validator : validators)
    {
        cout << validator.first << endl;
    }
    cout << "EOF" << endl;
    if (!validators["basic"]->validate(targetAdapter, &results))
    {
        // TODO Proper error reportingerror
        cout << "failed to validate basic" << endl;
        return;
    }
    cout << "getting event string" << endl;
    string event = d["event"].GetString();
    cout << "event: " << event << endl;
    try
    {
        if (!validators.at(event))
        {
            // TODO Proper error reporting
            cout << "failed to validate " << event << endl;
            return;
        }
    }
    catch (out_of_range &)
    {
        // TODO Proper error reporting
        cout << "schema not found " << event << endl;
        return;
    }
    cout << "Validated" << endl;
    if (event == "ready") {
        connection_open.raise(hdl);
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
