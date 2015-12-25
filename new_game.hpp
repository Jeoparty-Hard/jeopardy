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
    new_game(struct game_state_params *params);
    virtual ~new_game() = default;
    virtual void initialize();
    virtual bool process_event(const rapidjson::GenericValue<rapidjson::UTF8<>> &event);
    virtual void on_buzz(const buzzer &);
    virtual void current_state(rapidjson::Document &);
    virtual void store_state(rapidjson::Document &);
};

#endif //JEOPARDY_NEW_GAME_H
