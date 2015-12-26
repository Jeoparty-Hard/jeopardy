#include "new_game.hpp"

#include "invalid_event.hpp"
#include "jeopardy_exception.hpp"
#include "round_loader.hpp"
#include "setup_game.hpp"

using namespace std;
using namespace boost::filesystem;
using namespace rapidjson;

new_game::new_game(struct game_state_params *params)
    : game_state(params)
{
    for (jeopardy_round &round : round_loader::load_rounds())
    {
        rounds.emplace(round.get_id(), move(round));
    }
}

void new_game::initialize()
{
    players.clear();
    categories.clear();
}

bool new_game::process_event(const GenericValue<UTF8<>> &event)
{
    string event_name = event["event"].GetString();
    if (event_name == "select_round")
    {
        string round_id = event["round"].GetString();
        try
        {
            jeopardy_round &round = rounds.at(round_id);
            categories = round.get_categories();
            for (category &category : categories)
            {
                for (answer &answer : category.get_mutable_answers())
                {
                    answer.load_data();
                }
            }
            next_state.reset(new setup_game(params));
        }
        catch (out_of_range &)
        {
            throw jeopardy_exception("round '" + round_id + "' doesn't exist");
        }
        return true;
    }
    else
    {
        throw invalid_event();
    }
}

bool new_game::on_buzz(const buzzer &buzzer)
{
    return false;
}

void new_game::current_state(rapidjson::Document &d)
{
    d.SetObject();
    d.AddMember("state", "new", d.GetAllocator());
    Value rounds_value;
    rounds_value.SetObject();
    for (auto it = rounds.begin();it != rounds.end();it++)
    {
        jeopardy_round &round = rounds.at(it->first);
        rounds_value.AddMember(Value(round.get_id(), d.GetAllocator()), Value(round.get_name(), d.GetAllocator()), d.GetAllocator());
    }
    d.AddMember("rounds", rounds_value, d.GetAllocator());
}

void new_game::store_state(rapidjson::Document &root)
{
    root.AddMember("state", "new_game", root.GetAllocator());
    game_state::store_state(root);
}
