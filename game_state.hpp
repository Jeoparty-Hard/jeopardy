#ifndef JEOPARDY_GAME_STATE_H
#define JEOPARDY_GAME_STATE_H

#include <list>
#include <string>
#include <vector>

#include <rapidjson/document.h>

#include "player.hpp"
#include "category.hpp"
#include "websocket_server.hpp"

class game_state
{
protected:
    std::list<player> &players;
    std::vector<category> &categories;
    websocket_server &server;
public:
    game_state(std::list<player> *players, std::vector<category> *categories, websocket_server *server);
    virtual ~game_state() = default;
    virtual bool process_event(const rapidjson::GenericValue<rapidjson::UTF8<>> &event) = 0;
    virtual void current_state(rapidjson::Document &) = 0;
};

#endif //JEOPARDY_GAME_STATE_H
