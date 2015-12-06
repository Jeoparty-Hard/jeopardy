#include "game_state.hpp"

game_state::game_state(std::list<player> *players, std::vector<category> *categories, websocket_server *server, std::unique_ptr<game_state> *next_state)
    : players(*players),
      categories(*categories),
      server(*server),
      next_state(*next_state)
{
    // Nothing to do
}
