#include "player.hpp"

using namespace std;
using namespace rapidjson;

player::player(string id, string name, color c, const buzzer &buzzer)
    : mybuzzer(buzzer)
{
    this->id = id;
    this->name = name;
    this->c = c;
    this->score = 0;
    this->connected = true;
    this->buzzed = false;
}

const string & player::get_id() const
{
    return id;
}

const string & player::get_name() const
{
    return name;
}

color player::get_color() const
{
    return c;
}

int player::get_score() const
{
    return score;
}

bool player::is_connected() const
{
    return connected;
}

const buzzer& player::get_buzzer() const
{
    return mybuzzer;
}

bool player::has_buzzed() const
{
    return buzzed;
}

const std::chrono::duration<int, std::milli>& player::get_buzztime() const
{
    return buzztime;
}

GenericValue<UTF8<>> player::buzzed_value() const
{
    if (!buzzed)
        return Value();
    return Value(buzztime.count());
}
