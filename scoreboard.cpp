#include "scoreboard.hpp"

using namespace std;
using namespace rapidjson;

scoreboard::scoreboard(player *current_player, list<player> *players, vector<category> *categories, websocket_server *server, unique_ptr<game_state> *next_state)
        : game_state(players, categories, server, next_state)
{
    this->current_player = current_player;
}

scoreboard::scoreboard(list<player> *players, vector<category> *categories, websocket_server *server, unique_ptr<game_state> *next_state)
        : scoreboard(nullptr, players, categories, server, next_state)
{
    // Nothing to do
}

void scoreboard::initialize()
{
    if (current_player == nullptr)
        current_player = &*players.begin(); // Gets the address of the object the iterator points to
}

bool scoreboard::process_event(const GenericValue<UTF8<>> &event)
{
    return false;
}

void scoreboard::on_buzz(const buzzer &)
{

}

void scoreboard::current_state(rapidjson::Document &)
{

}
