#ifndef JEOPARDY_SCOREBOARD_H
#define JEOPARDY_SCOREBOARD_H

#include "game_state.hpp"

class scoreboard : public game_state
{
private:
    player *current_player;
public:
    scoreboard(struct game_state_params *params);
    scoreboard(player *current_player, struct game_state_params *params);
    virtual void initialize();
    virtual bool process_event(const rapidjson::GenericValue<rapidjson::UTF8<>> &event);
    virtual bool on_buzz(const buzzer &);
    virtual void current_state(rapidjson::Document &);
    virtual void store_state(rapidjson::Document &);
};

#endif //JEOPARDY_SCOREBOARD_H
