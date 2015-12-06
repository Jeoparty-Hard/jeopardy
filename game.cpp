#include "game.hpp"

#include <functional>

#include "new_game.hpp"

using namespace std;
using namespace rapidjson;

game::game(int port)
{
    state.reset(new new_game(&players, &categories, &server));
    server.connection_open.connect(bind(&game::on_client_connect, this, _1));
    server.start_listen(port);
    server.run();
}

void game::on_client_connect(websocketpp::connection_hdl hdl)
{
    Document d;
    state->current_state(d);
    server.send_document(hdl, d);
}
