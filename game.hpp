#ifndef JEOPARDY_GAME_CPP_H
#define JEOPARDY_GAME_CPP_H

#include <list>
#include <memory>

#include "category.hpp"
#include "game_state.hpp"
#include "mediator/buzzergroup_manager.hpp"
#include "player.hpp"
#include "websocket_server.hpp"

class game
{
private:
    std::unique_ptr<game_state> state;
    std::unique_ptr<game_state> next_state;
    std::list<player> players;
    std::vector<category> categories;
    buzzergroup_manager buzzer;
    websocket_server server;
public:
    game(int port);
    void on_client_connect(websocketpp::connection_hdl);
    void on_client_event(const rapidjson::GenericValue<rapidjson::UTF8<>> &);
    static void make_scoreboard(rapidjson::GenericValue<rapidjson::UTF8<>> &, const std::vector<category> &categories, rapidjson::GenericValue<rapidjson::UTF8<>>::AllocatorType &allocator);
    static void list_players(rapidjson::GenericValue<rapidjson::UTF8<>> &, const std::list<player> &players, rapidjson::GenericValue<rapidjson::UTF8<>>::AllocatorType &allocator);
};

#endif //JEOPARDY_GAME_CPP_H
