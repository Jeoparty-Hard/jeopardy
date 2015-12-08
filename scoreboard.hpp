#ifndef JEOPARDY_SCOREBOARD_H
#define JEOPARDY_SCOREBOARD_H

#include "game_state.hpp"

class scoreboard : public game_state
{
private:
    player *current_player;
public:
    scoreboard(std::list<player> *players, std::vector<category> *categories, websocket_server *server, std::unique_ptr<game_state> *next_state);
    scoreboard(player *current_player, std::list<player> *players, std::vector<category> *categories, websocket_server *server, std::unique_ptr<game_state> *next_state);
    virtual void initialize();
    virtual bool process_event(const rapidjson::GenericValue<rapidjson::UTF8<>> &event);
    virtual void on_buzz(const buzzer &);
    virtual void current_state(rapidjson::Document &);

};

#endif //JEOPARDY_SCOREBOARD_H
