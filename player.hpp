#ifndef JEOPARDY_PLAYER_H
#define JEOPARDY_PLAYER_H

#include <string>
#include <chrono>

#include <rapidjson/document.h>

#include "color.hpp"

class player
{
private:
    std::string id;
    std::string name;
    color c;
    int score;
    bool active;
    bool connected;
    bool buzzed;
    std::chrono::duration<int, std::milli> buzztime;
public:
    player(std::string id, std::string name, color);
    std::string get_id() const;
    std::string get_name() const;
    color get_color() const;
    int get_score() const;
    bool is_connected() const;
    rapidjson::GenericValue<rapidjson::UTF8<>> buzzed_value() const;
};

#endif //JEOPARDY_PLAYER_H
