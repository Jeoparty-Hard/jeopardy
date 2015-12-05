#ifndef JEOPARDY_GAME_CPP_H
#define JEOPARDY_GAME_CPP_H

#include <list>

#include "player.hpp"
#include "category.hpp"
#include "mediator/buzzergroup_manager.hpp"

class game
{
public:
    enum class state
    {
        NEW,
        SETUP,
        SCOREBOARD,
        ANSWER
    };
private:
    state state;
    std::list<player> players;
    std::vector<category> categories;
    buzzergroup_manager buzzer;
public:

};

#endif //JEOPARDY_GAME_CPP_H
