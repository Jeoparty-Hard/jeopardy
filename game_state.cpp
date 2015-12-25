#include "game_state.hpp"
#include "game_state_params.hpp"

using namespace std;
using namespace rapidjson;

game_state::game_state(struct game_state_params *params)
    : current_round(params->current_round),
      players(params->players),
      categories(params->categories),
      server(params->server),
      next_state(params->next_state),
      params(params)
{
    // Nothing to do
}

void game_state::store_state(Document &root)
{
    root.AddMember("round", Value(current_round.c_str(), current_round.size()), root.GetAllocator());

    Value playersValue;
    playersValue.SetArray();
    for (const player &current_player : players)
    {
        Value playerValue;
        current_player.store_state(playerValue, root.GetAllocator());
        playersValue.PushBack(playerValue, root.GetAllocator());
    }
    root.AddMember("players", playersValue, root.GetAllocator());

    Value winnersValue;
    winnersValue.SetArray();
    for (const category &category : categories)
    {
        Value categoryWinners;
        categoryWinners.SetArray();
        for (const answer &answer : category.get_answers())
        {
            categoryWinners.PushBack(answer.winner_value(), root.GetAllocator());
        }
        winnersValue.PushBack(categoryWinners, root.GetAllocator());
    }
    root.AddMember("winners", winnersValue, root.GetAllocator());
}
