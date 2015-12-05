#ifndef JEOPARDY_PLAYER_H
#define JEOPARDY_PLAYER_H

#include <string>
#include <chrono>

class player
{
private:
    std::string name;
    int score;
    bool active;
    bool connected;
    bool buzzed;
    std::chrono::duration<int, std::milli> buzztime;
public:
    player(std::string name);
};

#endif //JEOPARDY_PLAYER_H
