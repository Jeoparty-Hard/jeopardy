#include "player.hpp"

using namespace std;

player::player(string name)
{
    this->name = name;
    this->score = 0;
    this->active = true; // TODO Validate that this is a good default value
    this->buzzed = false;
}

