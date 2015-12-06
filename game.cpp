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
