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
public:
    setup_game(std::list<player> *players, std::vector<category> *categories, websocket_server *server, std::unique_ptr<game_state> *next_state);
    virtual ~setup_game() = default;
    virtual void initialize();
    virtual bool process_event(const rapidjson::GenericValue<rapidjson::UTF8<>> &event);
    virtual void current_state(rapidjson::Document &);
};

#endif //JEOPARDY_SETUP_GAME_H
