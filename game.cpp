#include "game.hpp"

#include <functional>

#include "jeopardy_exception.hpp"
#include "new_game.hpp"

using namespace std;
using namespace rapidjson;

game::game(int port)
{
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
    if (!state->process_event(event))
        throw jeopardy_exception("Event '" + string(event["event"].GetString()) + "' not allowed in this state");
    if (next_state) {
        state.reset(next_state.release());
        state->initialize();
        Document d;
        state->current_state(d);
        server.broadcast(d);
    }
}

void game::make_scoreboard(GenericValue<rapidjson::UTF8<>> &root, const std::vector<category> &categories, GenericValue<UTF8<>>::AllocatorType &allocator)
{
    root.SetObject();
    Value points;
    points.SetArray();
    bool first = true;
    Value categoriesValue;
    categoriesValue.SetArray();
    for (auto &category : categories)
    {
        Value categoryValue;
        categoryValue.SetObject();
        categoryValue.AddMember("name", Value(category.get_name().c_str(), category.get_name().size()), allocator);
        Value winners;
        winners.SetArray();
        for (auto &answer : category.get_answers())
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
    for (auto &player : players)
    {
        Value playerValue;
        playerValue.SetObject();
        playerValue.AddMember("name", Value(player.get_name().c_str(), player.get_name().size()), allocator);
        string playercolor = player.get_color().string();
        playerValue.AddMember("color", Value(playercolor.c_str(), playercolor.size()), allocator);
        playerValue.AddMember("score", player.get_score(), allocator);
        playerValue.AddMember("buzzed", player.buzzed_value(), allocator);
        playerValue.AddMember("connected", player.is_connected(), allocator);
        root.AddMember(Value(player.get_id().c_str(), player.get_id().size()), playerValue, allocator);
    }
    cout << endl;
}
