#include "answer_screen.hpp"

#include <algorithm>
#include <thread>

#include "game.hpp"
#include "scoreboard.hpp"

using namespace std;
using namespace std::chrono;
using namespace rapidjson;

answer_screen::answer_screen(answer *selected_answer, struct game_state_params *params)
    : game_state(params)
{
    this->selected_answer = selected_answer;
}

void answer_screen::initialize()
{
    for (player &player : players)
    {
        player.reset_buzztime();
    }
    unique_lock<recursive_mutex> lock(buzzorder_mutex);
    buzzorder.clear();
    start = high_resolution_clock::now();
}

bool answer_screen::process_event(const GenericValue<UTF8<>> &event)
{
    string event_type = event["event"].GetString();
    if (event_type == "win")
    {
        unique_lock<recursive_mutex> lock(buzzorder_mutex);
        if (buzzorder.size() == 0)
            return false;
        player *player = *buzzorder.begin();
        player->add_score(selected_answer->get_points());
        selected_answer->set_winner(player);
        next_state.reset(new scoreboard(player, params));
    }
    else if (event_type == "fail")
    {
        unique_lock<recursive_mutex> lock(buzzorder_mutex);
        if (buzzorder.size() == 0)
            return false;
        player *player = *buzzorder.begin();
        player->add_score(-selected_answer->get_points());
        // TODO Inform clients about new score
        initialize();
        send_buzzorder();
    }
    else if (event_type == "oops")
    {
        unique_lock<recursive_mutex> lock(buzzorder_mutex);
        if (buzzorder.size() == 0)
            return false;
        initialize();
        send_buzzorder();
    }
    else if (event_type == "exit")
    {
        unique_lock<recursive_mutex> lock(buzzorder_mutex);
        if (buzzorder.size() > 0)
            return false;
        player *next_player = nullptr;
        for (player &player : players)
        {
            if (next_player == nullptr)
                next_player = &player;
            else if (player.get_score() < next_player->get_score())
                next_player = &player;
        }
        selected_answer->set_winner(nullptr);
        next_state.reset(new scoreboard(next_player, params));
    }
    else
    {
        return false;
    }
    return true;
}

void answer_screen::on_buzz(const buzzer &hit_buzzer)
{
    duration<int, milli> time = duration_cast<milliseconds>(high_resolution_clock::now() - start);
    auto it = find_if(players.begin(), players.end(), [hit_buzzer](const class player &player){return player.is_connected() && player.get_buzzer() == hit_buzzer;});
    if (it == players.end())
        return;
    player &player = *it;
    if (player.has_buzzed())
        return;
    player.set_buzztime(time);
    unique_lock<recursive_mutex> lock(buzzorder_mutex);
    buzzorder.push_back(&player);
    send_buzzorder();
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
    make_buzzorder(buzzorderValue, d.GetAllocator());
    d.AddMember("buzzorder", buzzorderValue, d.GetAllocator());
}

void answer_screen::make_buzzorder(GenericValue<UTF8<>> &root, Document::AllocatorType &allocator)
{
    root.SetObject();
    bool first_buzz = true;
    duration<int, milli> first_time;
    unique_lock<recursive_mutex> lock(buzzorder_mutex);
    for (player *player : buzzorder)
    {
        if (!player->has_buzzed())
            continue;
        duration<int, milli> time;
        time = player->get_buzztime();
        if (first_buzz)
        {
            first_buzz = false;
            first_time = time;
        }
        else
        {
            time -= first_time;
        }
        root.AddMember(Value(player->get_id().c_str(), player->get_id().size()), Value(time.count()), allocator);
    }
}

void answer_screen::send_buzzorder()
{
    Document d;
    d.SetObject();
    Value buzzorderValue;
    make_buzzorder(buzzorderValue, d.GetAllocator());
    d.AddMember("buzzorder", buzzorderValue, d.GetAllocator());
    server.broadcast(d);
}

void answer_screen::store_state(rapidjson::Document &root)
{
    root.AddMember("state", "answer_screen", root.GetAllocator());
    game_state::store_state(root);
    root.AddMember("answer_col", selected_answer->get_col(), root.GetAllocator());
    root.AddMember("answer_row", selected_answer->get_row(), root.GetAllocator());
    Value buzzorderValue;
    buzzorderValue.SetArray();
    for (player *player : buzzorder)
    {
        buzzorderValue.PushBack(Value(player->get_id().c_str(), player->get_id().size()), root.GetAllocator());
    }
    root.AddMember("buzzorder", buzzorderValue, root.GetAllocator());
    root.AddMember("start", start.time_since_epoch().count(), root.GetAllocator());
}
