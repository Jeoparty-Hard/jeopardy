#ifndef JEOPARDY_SETUP_GAME_H
#define JEOPARDY_SETUP_GAME_H

#include "game_state.hpp"

class setup_game : public game_state
{
private:
    int next_player_id;
    bool edit_player_active;
    std::string current_playername;
    color playercolor;
    bool current_player_connected;
    buzzer current_player_buzzer;
public:
    setup_game(struct game_state_params *params);
    virtual ~setup_game() = default;
    virtual void initialize();
    virtual bool process_event(const rapidjson::GenericValue<rapidjson::UTF8<>> &event);
    virtual bool on_buzz(const buzzer &);
    virtual void current_state(rapidjson::Document &);
    virtual void store_state(rapidjson::Document &);
};

#endif //JEOPARDY_SETUP_GAME_H
