#include "setup_game.hpp"

#include "game.hpp"

using namespace std;
using namespace rapidjson;

setup_game::setup_game(list<player> *players, vector<category> *categories, websocket_server *server, unique_ptr<game_state> *next_state)
        : game_state(players, categories, server, next_state)
{
    this->next_player_id = 0;
    this->edit_player_active = false;
}

void setup_game::initialize()
{
    // Nothing to do
}

bool setup_game::process_event(const GenericValue<UTF8<>> &event)
{
    string event_type = event["event"].GetString();
    if (event_type == "add_player")
    {
        if (edit_player_active)
            return false;
        edit_player_active = true;
        current_playername = "";
        current_player_connected = true;
        playercolor = color(event["color"].GetString());
        Document d;
        current_state(d);
        server.broadcast(d);
    }
    else if (event_type == "update_player_name")
    {
        if (!edit_player_active)
            return false;
        current_playername = event["name"].GetString();
        Document d;
        current_state(d);
        server.broadcast(d);
    }
    else if (event_type == "confirm_player")
    {
        if (!edit_player_active)
            return false;
        // TODO Check if player is attached before confirming
        edit_player_active = false;
        players.emplace_back(to_string(next_player_id++), current_playername, color(playercolor));
        Document d;
        current_state(d);
        server.broadcast(d);
    }
    else
    {
        return false;
    }
    return true;
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
    Value new_player;
    if (edit_player_active)
    {
        new_player.SetObject();
        new_player.AddMember("name", Value(current_playername.c_str(), current_playername.size()), d.GetAllocator());
        new_player.AddMember("color", Value(playercolor.string().c_str(), playercolor.string().size()), d.GetAllocator());
        new_player.AddMember("connected", current_player_connected, d.GetAllocator());
    }
    d.AddMember("new_player", new_player, d.GetAllocator());
}
