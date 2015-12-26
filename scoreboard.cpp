#include "scoreboard.hpp"

#include "answer_screen.hpp"
#include "game.hpp"
#include "invalid_event.hpp"
#include "jeopardy_exception.hpp"

using namespace std;
using namespace rapidjson;

scoreboard::scoreboard(player *current_player, struct game_state_params *params)
        : game_state(params)
{
    this->current_player = current_player;
}

scoreboard::scoreboard(struct game_state_params *params)
        : scoreboard(nullptr, params)
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
            next_state.reset(new answer_screen(&categories.at(category_id).get_mutable_answers().at(answer_id), params));
        }
        catch (out_of_range &)
        {
            throw jeopardy_exception("Answer " + to_string(category_id) + "," + to_string(answer_id) + " does not exist");
        }
        return true;
    }
    else
    {
        throw invalid_event();
    }
}

bool scoreboard::on_buzz(const buzzer &)
{
    return false;
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
    d.AddMember("current_player", Value(current_player->get_id(), d.GetAllocator()), d.GetAllocator());
}

void scoreboard::store_state(rapidjson::Document &root)
{
    root.AddMember("state", "scoreboard", root.GetAllocator());
    game_state::store_state(root);
    root.AddMember("current_player", Value(current_player->get_id(), root.GetAllocator()), root.GetAllocator());
}
