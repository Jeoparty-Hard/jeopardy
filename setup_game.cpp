#include "setup_game.hpp"

#include "game.hpp"

using namespace rapidjson;

setup_game::setup_game(std::list<player> *players, std::vector<category> *categories, websocket_server *server, std::unique_ptr<game_state> *next_state)
        : game_state(players, categories, server, next_state)
{
    // Nothing to do
}

void setup_game::initialize()
{
    // Nothing to do
}

bool setup_game::process_event(const rapidjson::GenericValue<rapidjson::UTF8<>> &event)
{
    // TODO Implement
}

void setup_game::current_state(rapidjson::Document &d)
{
    d.SetObject();
    d.AddMember("state", "setup", d.GetAllocator());
    Value scoreboard;
    game::make_scoreboard(scoreboard, categories, d.GetAllocator());
    d.AddMember("scoreboard", scoreboard, d.GetAllocator());
    Value playersValue;
    game::list_players(playersValue, players, d.GetAllocator());
    d.AddMember("players", playersValue, d.GetAllocator());
}
