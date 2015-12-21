#include "game.hpp"

#include <functional>

#include "jeopardy_exception.hpp"
#include "new_game.hpp"

using namespace std;
using namespace rapidjson;

game::game(int port)
{
    this->reconnect_player = nullptr;
    buzzer_manager.buzzer_hit.connect(bind(&game::on_buzzer_hit, this, _1));
    buzzer_manager.buzzer_disconnected.connect(bind(&game::on_buzzer_disconnected, this, _1));
    buzzer_manager.buzzergroup_connected.connect(bind(&game::on_buzzergroup_connected, this, _1, _2));
    buzzer_manager.buzzergroup_connect_failed.connect(bind(&game::on_buzzergroup_connect_failed, this, _1, _2));
    buzzer_manager.buzzergroup_disconnected.connect(bind(&game::on_buzzergroup_disconnected, this, _1, _2));
    state.reset(new new_game(&players, &categories, &server, &next_state));
    server.connection_open.connect(bind(&game::on_client_connect, this, _1));
    server.client_event.connect(bind(&game::on_client_event, this, _1));
    server.start_listen(port);
    server.run();
}

void game::on_client_connect(websocketpp::connection_hdl hdl)
{
    Document d;
    state->current_state(d);
    server.send_document(hdl, d);
}

void game::on_client_event(const GenericValue<UTF8<>> &event)
{
    string event_type = event["event"].GetString();
    if (event_type == "connect_buzzergroup")
    {
        string device_type_string = event["type"].GetString();
        device_type type;
        if (device_type_string == "serial")
        {
            type = device_type::SERIAL;
        }
        else if (device_type_string == "keyboard")
        {
            type = device_type::KEYBOARD;
        }
        else
        {
            throw jeopardy_exception("Unexpected buzzergroup type '" + device_type_string + "'");
        }
        buzzer_manager.connect(event["device"].GetString(), type);
        return;
    }
    if (event_type == "reconnect")
    {
        string player_id = event["player"].GetString();
        auto it = find_if(players.begin(), players.end(), [player_id](const player &player){return player.get_id() == player_id;});
        if (it == players.end())
            return;
        player &player = *it;
        if (player.is_connected())
            throw jeopardy_exception("Player '" + player.get_name() + "' is already connected");
        if (reconnect_player != nullptr)
            throw jeopardy_exception("Player '" + reconnect_player->get_name() + "' is currently connecting");
        reconnect_player = &player;
        return;
    }
    if (event_type == "refresh")
    {
        Document d;
        state->current_state(d);
        server.broadcast(d);
        return;
    }
    if (!state->process_event(event))
        throw jeopardy_exception("Event '" + string(event["event"].GetString()) + "' not allowed in this state");
    if (next_state)
    {
        state.reset(next_state.release());
        state->initialize();
        Document d;
        state->current_state(d);
        server.broadcast(d);
    }
}

void game::on_buzzer_hit(const buzzer &buzzer_hit)
{
    if (reconnect_player != nullptr)
    {
        auto it = find_if(players.begin(), players.end(), [buzzer_hit](const player &player){return player.is_connected() && player.get_buzzer() == buzzer_hit;});
        if (it == players.end())
        {
            reconnect_player->set_buzzer(buzzer_hit);
            reconnect_player = nullptr;
            Document d;
            state->current_state(d);
            server.broadcast(d);
            return;
        }
    }
    state->on_buzz(buzzer_hit);
}

void game::on_buzzer_disconnected(const buzzer &disconnected_buzzer)
{
    auto it = find_if(players.begin(), players.end(), [disconnected_buzzer](const player &player){return player.is_connected() && player.get_buzzer() == disconnected_buzzer;});
    if (it != players.end())
        return;
    player &player = *it;
    player.disconnect();
    Document d;
    state->current_state(d);
    server.broadcast(d);
}

void game::on_buzzergroup_connected(std::string device, const std::set<unsigned char> &buzzer_ids)
{
    Document d;
    d.SetObject();
    d.AddMember("connection", "successfull", d.GetAllocator());
    d.AddMember("device", Value(device.c_str(), device.size()), d.GetAllocator());
    server.broadcast(d);
}

void game::on_buzzergroup_connect_failed(std::string device, std::string error_message)
{
    Document d;
    d.SetObject();
    d.AddMember("error", "connect_failed", d.GetAllocator());
    d.AddMember("device", Value(device.c_str(), device.size()), d.GetAllocator());
    d.AddMember("message", Value(error_message.c_str(), error_message.size()), d.GetAllocator());
    server.broadcast(d);
}

void game::on_buzzergroup_disconnected(std::string device, disconnect_reason reason)
{
    for (player &player : players)
    {
        if (!player.is_connected() || player.get_buzzer().device != device)
            continue;
        player.disconnect();
    }
    Document d;
    state->current_state(d);
    server.broadcast(d);
}

void game::make_scoreboard(GenericValue<rapidjson::UTF8<>> &root, const std::vector<category> &categories, GenericValue<UTF8<>>::AllocatorType &allocator)
{
    root.SetObject();
    Value points;
    points.SetArray();
    bool first = true;
    Value categoriesValue;
    categoriesValue.SetArray();
    for (const category &category : categories)
    {
        Value categoryValue;
        categoryValue.SetObject();
        categoryValue.AddMember("name", Value(category.get_name().c_str(), category.get_name().size()), allocator);
        Value winners;
        winners.SetArray();
        for (const answer &answer : category.get_answers())
        {
            winners.PushBack(answer.winner_value(), allocator);
            if (first)
                points.PushBack(answer.get_points(), allocator);
        }
        first = false;
        categoryValue.AddMember("winner", winners, allocator);
        categoriesValue.PushBack(categoryValue, allocator);
    }
    root.AddMember("points", points, allocator);
    root.AddMember("categories", categoriesValue, allocator);
}

void game::list_players(GenericValue<rapidjson::UTF8<>> &root, const std::list<player> &players, GenericValue<UTF8<>>::AllocatorType &allocator)
{
    root.SetObject();
    for (const player &player : players)
    {
        Value playerValue;
        playerValue.SetObject();
        playerValue.AddMember("name", Value(player.get_name().c_str(), player.get_name().size()), allocator);
        playerValue.AddMember("color", Value(player.get_color().string().c_str(), player.get_color().string().size()), allocator);
        playerValue.AddMember("score", player.get_score(), allocator);
        playerValue.AddMember("buzzed", player.buzzed_value(), allocator);
        playerValue.AddMember("connected", player.is_connected(), allocator);
        root.AddMember(Value(player.get_id().c_str(), player.get_id().size()), playerValue, allocator);
    }
}
