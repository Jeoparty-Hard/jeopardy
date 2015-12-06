#include "game_state.hpp"

game_state::game_state(std::list<player> *players, std::vector<category> *categories, websocket_server *server)
    : players(*players),
      categories(*categories),
      server(*server)
{
    // Nothing to do
}
