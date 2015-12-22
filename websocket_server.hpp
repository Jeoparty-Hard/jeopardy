#ifndef JEOPARDY_WEBSOCKET_SERVER_H
#define JEOPARDY_WEBSOCKET_SERVER_H

#include <set>
#include <string>
#include <map>
#include <memory>

#include <rapidjson/document.h>
#include <websocketpp/common/connection_hdl.hpp>

#include "mediator/event.hpp"

class websocket_server
{
private:
    struct data_t;
    std::unique_ptr<data_t> data;
    std::set<websocketpp::connection_hdl, std::owner_less<websocketpp::connection_hdl>> connections;
    void on_connection_open(websocketpp::connection_hdl hdl);
    void on_connection_close(websocketpp::connection_hdl hdl);
public:
    event<websocketpp::connection_hdl> connection_open;
    event<const rapidjson::GenericValue<rapidjson::UTF8<>> &> client_event;
    websocket_server();
    ~websocket_server();
    void start_listen(int port);
    void shutdown();
    void send_document(websocketpp::connection_hdl hdl, const rapidjson::Document &);
    void broadcast(const rapidjson::Document &);
    void run();
};

#endif //JEOPARDY_WEBSOCKET_SERVER_H
