#include "player.hpp"

using namespace std;
using namespace rapidjson;

player::player(string name)
{
    this->name = name;
    this->score = 0;
    this->active = true; // TODO Validate that this is a good default value
    this->buzzed = false;
}

std::string player::get_name() const
{
    return name;
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
