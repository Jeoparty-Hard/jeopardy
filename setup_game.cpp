#include "setup_game.hpp"

#include "game.hpp"
#include "jeopardy_exception.hpp"
#include "scoreboard.hpp"

using namespace std;
using namespace rapidjson;

setup_game::setup_game(struct game_state_params *params)
        : game_state(params)
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
        current_player_connected = false;
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
        if (!current_player_connected)
            throw jeopardy_exception("The player doesn't have a buzzer assigned");
        edit_player_active = false;
        players.emplace_back(to_string(next_player_id++), current_playername, color(playercolor), current_player_buzzer);
        Document d;
        current_state(d);
        server.broadcast(d);
    }
    else if (event_type == "start")
    {
        if (edit_player_active)
            return false;
        if (players.size() < 1)
            throw jeopardy_exception("At least one player is required to start the game");
        next_state.reset(new scoreboard(params));
    }
    else
    {
        return false;
    }
    return true;
}

void setup_game::on_buzz(const buzzer &buzzer)
{
    if (!edit_player_active)
        return;
    if (current_player_connected)
        return;
    if (find_if(players.begin(), players.end(), [buzzer](const player &player){return player.is_connected() && buzzer == player.get_buzzer();}) != players.end())
        return;
    current_player_connected = true;
    current_player_buzzer = buzzer;
    Document d;
    current_state(d);
    server.broadcast(d);
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

void setup_game::store_state(rapidjson::Document &root)
{
    root.AddMember("state", "setup_game", root.GetAllocator());
    game_state::store_state(root);
}
