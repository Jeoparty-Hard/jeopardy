#include "player.hpp"

using namespace std;
using namespace rapidjson;

player::player(string id, string name, color c)
    : c(c)
{
    this->id = id;
    this->name = name;
    this->score = 0;
    this->active = true; // TODO Validate that this is a good default value
    this->buzzed = false;
}

std::string player::get_id() const
{
    return id;
}

std::string player::get_name() const
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

GenericValue<UTF8<>> player::buzzed_value() const
{
    if (!buzzed)
        return Value();
    return Value(buzztime.count());
}
