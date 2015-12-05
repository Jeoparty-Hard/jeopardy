#include "websocket_server.hpp"

#include <functional>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

using namespace std;
using namespace std::placeholders;
using namespace rapidjson;
using namespace websocketpp;

websocket_server::websocket_server()
{
    this->init_asio();
    this->set_open_handler(bind(&websocket_server::on_connection_open, this, _1));
}

void websocket_server::start_listen(int port)
{
    // TODO Reuse address
    this->listen(port);
    this->start_accept();
}

void websocket_server::on_message(connection_hdl, message_ptr)
{
    // TODO Implement
}

void websocket_server::on_connection_open(connection_hdl hdl)
{
    // TODO Handshake?
    connections.insert(hdl);
}

void websocket_server::on_connection_close(connection_hdl hdl)
{
    connections.erase(hdl);
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
