#include "answer_screen.hpp"

#include <algorithm>
#include <thread>

#include "game.hpp"
#include "invalid_event.hpp"
#include "scoreboard.hpp"

using namespace std;
using namespace std::chrono;
using namespace rapidjson;

answer_screen::answer_screen(answer *selected_answer, struct game_state_params *params)
    : game_state(params)
{
    this->selected_answer = selected_answer;
}

answer_screen::answer_screen(const GenericValue<UTF8<>> &root, struct game_state_params *params)
    : game_state(root, params)
{
    try
    {
        this->selected_answer = &categories.at(root["answer_col"].GetInt()).get_mutable_answers().at(root["answer_row"].GetInt());
    }
    catch (out_of_range &)
    {
        throw invalid_json("Illegal answer");
    }
    auto &buzzorder_value = root["buzzorder"];
    for (unsigned int i = 0;i < buzzorder_value.Capacity();i++)
    {
        string player_id = buzzorder_value[i].GetString();
        auto it = find_if(players.begin(), players.end(), [player_id](const player &player){return player.get_id() == player_id;});
        if (it == players.end())
            throw invalid_json("Buzzorder player does not exist");
        buzzorder.push_back(&*it);
    }
    start = time_point<high_resolution_clock>(time_point<high_resolution_clock>::duration(root["start"].GetInt64()));
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
            throw invalid_event();
        player *player = *buzzorder.begin();
        player->add_score(selected_answer->get_points());
        selected_answer->set_winner(player);
        next_state.reset(new scoreboard(player, params));
        return true;
    }
    else if (event_type == "fail")
    {
        unique_lock<recursive_mutex> lock(buzzorder_mutex);
        if (buzzorder.size() == 0)
            throw invalid_event();
        player *player = *buzzorder.begin();
        player->add_score(-selected_answer->get_points());
        // TODO Inform clients about new score
        initialize();
        Document d;
        current_state(d, false);
        server.broadcast(d);
        return true;
    }
    else if (event_type == "oops")
    {
        unique_lock<recursive_mutex> lock(buzzorder_mutex);
        if (buzzorder.size() == 0)
            throw invalid_event();
        initialize();
        Document d;
        current_state(d, false);
        server.broadcast(d);
        return true;
    }
    else if (event_type == "exit")
    {
        unique_lock<recursive_mutex> lock(buzzorder_mutex);
        if (buzzorder.size() > 0)
            throw invalid_event();
        player *next_player = &*min_element(players.begin(), players.end(), [](const player &player1, const player &player2){return player1.get_score() < player2.get_score();});
        if (!selected_answer->is_won())
            selected_answer->set_winner(nullptr);
        next_state.reset(new scoreboard(next_player, params));
        return true;
    }
    else
    {
        throw invalid_event();
    }
}

bool answer_screen::on_buzz(const buzzer &hit_buzzer)
{
    duration<int, milli> time = duration_cast<milliseconds>(high_resolution_clock::now() - start);
    auto it = find_if(players.begin(), players.end(), [hit_buzzer](const class player &player){return player.is_connected() && player.get_buzzer() == hit_buzzer;});
    if (it == players.end())
        return false;
    player &player = *it;
    if (player.has_buzzed())
        return false;
    player.set_buzztime(time);
    unique_lock<recursive_mutex> lock(buzzorder_mutex);
    buzzorder.push_back(&player);
    Document d;
    current_state(d, false);
    server.broadcast(d);
    return true;
}

void answer_screen::current_state(rapidjson::Document &d)
{
    current_state(d, true);
}

void answer_screen::current_state(Document &d, bool send_data)
{
    d.SetObject();
    d.AddMember("state", "answer", d.GetAllocator());
    Value answer;
    answer.SetObject();
    answer.AddMember("type", Value(selected_answer->get_type(), d.GetAllocator()), d.GetAllocator());
    if (send_data)
        answer.AddMember("data", Value(selected_answer->get_data(), d.GetAllocator()), d.GetAllocator());
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
    root.SetArray();
    bool first_buzz = true;
    duration<int, milli> first_time;
    unique_lock<recursive_mutex> lock(buzzorder_mutex);
    for (player *player : buzzorder)
    {
        Value buzzorderEntry;
        buzzorderEntry.SetObject();
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
        buzzorderEntry.AddMember("id", Value(player->get_id(), allocator), allocator);
        buzzorderEntry.AddMember("time", time.count(), allocator);
        root.PushBack(buzzorderEntry, allocator);
    }
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
        buzzorderValue.PushBack(Value(player->get_id(), root.GetAllocator()), root.GetAllocator());
    }
    root.AddMember("buzzorder", buzzorderValue, root.GetAllocator());
    root.AddMember("start", start.time_since_epoch().count(), root.GetAllocator());
}
