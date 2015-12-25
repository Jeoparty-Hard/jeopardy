#ifndef JEOPARDY_GAME_STATE_PARAMS_H
#define JEOPARDY_GAME_STATE_PARAMS_H

#include <memory>
#include <string>
#include <vector>

#include "player.hpp"
#include "category.hpp"
#include "game_state.hpp"
#include "websocket_server.hpp"

struct game_state_params
{
    std::string current_round;
    std::list<player> players;
    std::vector<category> categories;
    websocket_server server;
    std::unique_ptr<game_state> next_state;
};

#endif //JEOPARDY_GAME_STATE_PARAMS_H
