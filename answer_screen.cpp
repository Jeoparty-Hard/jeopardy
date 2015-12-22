#include "answer_screen.hpp"

#include <algorithm>
#include <thread>

#include "game.hpp"
#include "scoreboard.hpp"

using namespace std;
using namespace std::chrono;
using namespace rapidjson;

answer_screen::answer_screen(answer *selected_answer, list<player> *players, vector<category> *categories, websocket_server *server, unique_ptr<game_state> *next_state)
    : game_state(players, categories, server, next_state)
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
        next_state.reset(new scoreboard(player, &players, &categories, &server, &next_state));
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
        next_state.reset(new scoreboard(next_player, &players, &categories, &server, &next_state));
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
    player &player = *find_if(players.begin(), players.end(), [hit_buzzer](const class player &player){return player.is_connected() && player.get_buzzer() == hit_buzzer;});
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
