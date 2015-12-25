#ifndef JEOPARDY_GAME_STATE_H
#define JEOPARDY_GAME_STATE_H

#include <list>
#include <string>
#include <vector>

#include <rapidjson/document.h>

#include "category.hpp"
#include "player.hpp"
#include "websocket_server.hpp"
#include "mediator/buzzer.hpp"

struct game_state_params;

class game_state
{
protected:
    std::string &current_round;
    std::list<player> &players;
    std::vector<category> &categories;
    websocket_server &server;
    std::unique_ptr<game_state> &next_state;
    struct game_state_params *params;
public:
    game_state(struct game_state_params *);
    virtual ~game_state() = default;
    virtual void initialize() = 0;
    virtual bool process_event(const rapidjson::GenericValue<rapidjson::UTF8<>> &event) = 0;
    virtual void on_buzz(const buzzer &) = 0;
    virtual void current_state(rapidjson::Document &) = 0;
    virtual void store_state(rapidjson::Document &d);
};

#endif //JEOPARDY_GAME_STATE_H
