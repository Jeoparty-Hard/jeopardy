#ifndef JEOPARDY_NEW_GAME_H
#define JEOPARDY_NEW_GAME_H

#include <map>
#include <string>

#include "game_state.hpp"
#include "jeopardy_round.hpp"

class new_game : public game_state
{
private:
    std::map<std::string, jeopardy_round> rounds;
public:
    new_game(std::list<player> *players, std::vector<category> *categories, websocket_server *server);
    virtual ~new_game() = default;
    virtual bool process_event(const rapidjson::GenericValue<rapidjson::UTF8<>> &event);
    virtual void current_state(rapidjson::Document &);
};

#endif //JEOPARDY_NEW_GAME_H
