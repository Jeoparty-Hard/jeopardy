#ifndef JEOPARDY_WEBSOCKET_SERVER_H
#define JEOPARDY_WEBSOCKET_SERVER_H

#include <set>

#include <rapidjson/document.h>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

class websocket_server : public websocketpp::server<websocketpp::config::asio>
{
private:
    std::set<websocketpp::connection_hdl, std::owner_less<websocketpp::connection_hdl>> connections;
    void on_connection_open(websocketpp::connection_hdl hdl);
    void on_connection_close(websocketpp::connection_hdl hdl);
    void on_message(websocketpp::connection_hdl, message_ptr);
public:
    websocket_server();
    void start_listen(int port);
    void shutdown();
    void broadcast(const rapidjson::Document &);
};

#endif //JEOPARDY_WEBSOCKET_SERVER_H
