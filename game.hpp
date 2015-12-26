#ifndef JEOPARDY_GAME_CPP_H
#define JEOPARDY_GAME_CPP_H

#include <list>
#include <memory>

#include "category.hpp"
#include "game_state.hpp"
#include "mediator/buzzergroup_manager.hpp"
#include "player.hpp"
#include "game_state_params.hpp"
#include "websocket_server.hpp"

class game
{
private:
    struct game_state_params data;
    std::unique_ptr<game_state> state;
    buzzergroup_manager buzzer_manager;
    player *reconnect_player;
    unsigned int next_state_file_id;
    void on_client_connect(websocketpp::connection_hdl);
    void on_client_event(const rapidjson::GenericValue<rapidjson::UTF8<>> &);
    void on_buzzergroup_connected(std::string device, const std::set<unsigned char> &buzzer_ids);
    void on_buzzergroup_disconnected(std::string device, disconnect_reason reason);
    void on_buzzergroup_connect_failed(std::string device, std::string error_message);
    void on_buzzer_disconnected(const buzzer &);
    void on_buzzer_hit(const buzzer &);
    void store_state();
    bool process_client_event(const rapidjson::GenericValue<rapidjson::UTF8<>> &);
    bool process_buzzer_hit(const buzzer &);
public:
    game(int port);
    static void make_scoreboard(rapidjson::GenericValue<rapidjson::UTF8<>> &, const std::vector<category> &categories, rapidjson::GenericValue<rapidjson::UTF8<>>::AllocatorType &allocator);
    static void list_players(rapidjson::GenericValue<rapidjson::UTF8<>> &, const std::list<player> &players, rapidjson::GenericValue<rapidjson::UTF8<>>::AllocatorType &allocator);
};

#endif //JEOPARDY_GAME_CPP_H
