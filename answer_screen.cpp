#include "answer_screen.hpp"

#include "game.hpp"

using namespace std;
using namespace std::chrono;
using namespace rapidjson;

answer_screen::answer_screen(const answer *selected_answer, list<player> *players, vector<category> *categories, websocket_server *server, unique_ptr<game_state> *next_state)
    : game_state(players, categories, server, next_state)
{
    this->selected_answer = selected_answer;
}

void answer_screen::initialize()
{
    start = high_resolution_clock::now();
}

bool answer_screen::process_event(const GenericValue<UTF8<>> &event)
{
    return false;
}

void answer_screen::on_buzz(const buzzer &)
{
    // TODO Implement
}

void answer_screen::current_state(Document &d)
{
    d.SetObject();
    d.AddMember("state", "answer", d.GetAllocator());
    Value answer;
    answer.SetObject();
    answer.AddMember("type", Value(selected_answer->get_type().c_str(), selected_answer->get_type().size()), d.GetAllocator());
    answer.AddMember("data", Value(selected_answer->get_data().c_str(), selected_answer->get_data().size()), d.GetAllocator());
    answer.AddMember("score", selected_answer->get_points(), d.GetAllocator());
    d.AddMember("answer", answer, d.GetAllocator());
    Value playersValue;
    game::list_players(playersValue, players, d.GetAllocator());
    d.AddMember("players", playersValue, d.GetAllocator());
    Value buzzorderValue;
    buzzorderValue.SetObject();
    bool first_buzz = true;
    duration<int, milli> first_time;
    for (auto &player : buzzorder)
    {
        if (!player.has_buzzed())
            continue;
        duration<int, milli> time;
        time = player.get_buzztime();
        if (first_buzz)
        {
            first_buzz = false;
            first_time = time;
        }
        else
        {
            time -= first_time;
        }
        buzzorderValue.AddMember(Value(player.get_id().c_str(), player.get_id().size()), Value(time.count()), d.GetAllocator());
    }
    d.AddMember("buzzorder", buzzorderValue, d.GetAllocator());
}
