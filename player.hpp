#ifndef JEOPARDY_PLAYER_H
#define JEOPARDY_PLAYER_H

#include <string>
#include <chrono>

#include <rapidjson/document.h>

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
    std::string get_name() const;
    int get_score() const;
    bool is_connected() const;
    rapidjson::GenericValue<rapidjson::UTF8<>> buzzed_value() const;
};

#endif //JEOPARDY_PLAYER_H
