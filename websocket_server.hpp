#ifndef JEOPARDY_WEBSOCKET_SERVER_H
#define JEOPARDY_WEBSOCKET_SERVER_H

#include <set>
#include <string>
#include <map>
#include <memory>

#include <rapidjson/document.h>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <valijson/validator.hpp>

#include "mediator/event.hpp"

class websocket_server : public websocketpp::server<websocketpp::config::asio>
{
private:
    std::set<websocketpp::connection_hdl, std::owner_less<websocketpp::connection_hdl>> connections;
    std::map<std::string, std::unique_ptr<valijson::Validator>> validators;
    void on_connection_open(websocketpp::connection_hdl hdl);
    void on_connection_close(websocketpp::connection_hdl hdl);
    void on_message(websocketpp::connection_hdl, message_ptr);
public:
    event<websocketpp::connection_hdl> connection_open;
    event<const rapidjson::GenericValue<rapidjson::UTF8<>> &> client_event;
    websocket_server();
    void start_listen(int port);
    void shutdown();
    void send_document(websocketpp::connection_hdl hdl, const rapidjson::Document &);
    void broadcast(const rapidjson::Document &);
};

#endif //JEOPARDY_WEBSOCKET_SERVER_H
