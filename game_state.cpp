#include "game_state.hpp"
#include "game_state_params.hpp"

#include "invalid_json.hpp"
#include "data_loader.hpp"

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

game_state::game_state(const rapidjson::GenericValue<rapidjson::UTF8<>> &json, struct game_state_params *params)
    : current_round(params->current_round),
      players(params->players),
      categories(params->categories),
      server(params->server),
      next_state(params->next_state),
      params(params)
{
    auto &players_array = json["players"];
    for (unsigned int i = 0;i < players_array.Capacity();i++)
    {
        players.emplace_back(players_array[i]);
    }
    current_round = json["round"].GetString();
    if  (current_round.size() > 0)
    {
        jeopardy_round round = data_loader::load_round(current_round);
        categories = round.get_categories();
        auto &winners = json["winners"];
        if (categories.size() != winners.Capacity())
            throw invalid_json("winners array capacity does not equal number of categories in round '" + current_round + "'");
        for (unsigned int category_no = 0; category_no < categories.size(); category_no++)
        {
            vector<answer> &answers = categories[category_no].get_mutable_answers();
            auto &category_winners = winners[category_no];
            if (answers.size() != category_winners.Capacity())
                throw invalid_json("number of winners in category " + to_string(category_no) + " doesn't equal number of answers");
            for (unsigned int answer_no = 0; answer_no < answers.size(); answer_no++)
            {
                answer &answer = answers[answer_no];
                auto &winner = category_winners[answer_no];
                if (!winner.IsNull())
                {
                    if (winner.IsBool())
                    {
                        if (winner.GetBool() == true)
                            throw invalid_json("winner values cannot be true. they must be either null, false or a string");
                        answer.set_winner(nullptr);
                    }
                    else
                    {
                        string winner_id = winner.GetString();
                        auto it = find_if(players.begin(), players.end(), [winner_id](player &player) { return player.get_id() == winner_id; });
                        if (it == players.end())
                            throw invalid_json(string("no player with id '") + winner.GetString() + "' but id is used in winners");
                        answer.set_winner(&*it);
                    }
                }
                answer.load_data();
            }
        }
    }
}

void game_state::store_state(Document &root)
{
    root.AddMember("round", Value(current_round, root.GetAllocator()), root.GetAllocator());

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
            categoryWinners.PushBack(answer.winner_value(root.GetAllocator()), root.GetAllocator());
        }
        winnersValue.PushBack(categoryWinners, root.GetAllocator());
    }
    root.AddMember("winners", winnersValue, root.GetAllocator());
}
