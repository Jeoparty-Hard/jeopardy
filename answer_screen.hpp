#ifndef JEOPARDY_ANSWER_SCREEN_H
#define JEOPARDY_ANSWER_SCREEN_H

#include <chrono>

#include "game_state.hpp"

class answer_screen : public game_state
{
private:
    std::list<player*> buzzorder;
    const answer *selected_answer;
    std::chrono::time_point<std::chrono::high_resolution_clock> start;
public:
    answer_screen(const answer *selected_answer, std::list<player> *players, std::vector<category> *categories, websocket_server *server, std::unique_ptr<game_state> *next_state);
    virtual ~answer_screen() = default;
    virtual void initialize();
    virtual bool process_event(const rapidjson::GenericValue<rapidjson::UTF8<>> &event);
    virtual void on_buzz(const buzzer &);
    virtual void current_state(rapidjson::Document &);
};

#endif //JEOPARDY_ANSWER_SCREEN_H
