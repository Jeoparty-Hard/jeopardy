#include "answer_screen.hpp"

using namespace std;
using namespace rapidjson;

answer_screen::answer_screen(answer *selected_answer, list<player> *players, vector<category> *categories, websocket_server *server, unique_ptr<game_state> *next_state)
    : game_state(players, categories, server, next_state)
{
    this->selected_answer = selected_answer;
}

void answer_screen::initialize()
{
    // Nothing to do
}

bool answer_screen::process_event(const GenericValue<UTF8<>> &event)
{
    return false;
}

void answer_screen::on_buzz(const buzzer &)
{
    // TODO Implement
}

void answer_screen::current_state(Document &)
{
    // TODO Implement
}
