#include "scoreboard.hpp"

#include "answer_screen.hpp"
#include "game.hpp"
#include "jeopardy_exception.hpp"

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
    string event_type = event["event"].GetString();
    if (event_type == "select_answer")
    {
        int category_id = event["category"].GetInt();
        int answer_id = event["answer"].GetInt();
        try
        {
        }
        catch (out_of_range &)
        {
            throw jeopardy_exception("Answer " + to_string(category_id) + "," + to_string(answer_id) + " does not exist");
        }
    }
    else
    {
        return false;
    }
    return true;
}

void scoreboard::on_buzz(const buzzer &)
{
    // Nothing to do
}

void scoreboard::current_state(rapidjson::Document &d)
{
    d.SetObject();
    d.AddMember("state", "scoreboard", d.GetAllocator());
    Value scoreboardValue;
    game::make_scoreboard(scoreboardValue, categories, d.GetAllocator());
    d.AddMember("scoreboard", scoreboardValue, d.GetAllocator());
    Value playersValue;
    game::list_players(playersValue, players, d.GetAllocator());
    d.AddMember("players", playersValue, d.GetAllocator());
    d.AddMember("current_player", Value(current_player->get_name().c_str(), current_player->get_name().size()), d.GetAllocator());
}
