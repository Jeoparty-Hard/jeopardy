#include "player.hpp"

using namespace std;
using namespace rapidjson;

player::player(string id, string name, color c, const buzzer &buzzer)
    : mybuzzer(buzzer)
{
    this->id = id;
    this->name = name;
    this->c = c;
    this->score = 0;
    this->connected = true;
    this->buzzed = false;
}

void player::set_buzztime(const std::chrono::duration<int, std::milli> &buzztime)
{
    this->buzzed = true;
    this->buzztime = buzztime;
}

void player::reset_buzztime()
{
    this->buzzed = false;
}

void player::add_score(int score)
{
    this->score += score;
}

void player::set_buzzer(const buzzer &mybuzzer)
{
    this->mybuzzer = mybuzzer;
    this->connected = true;
}

void player::disconnect()
{
    this->connected = false;
}

const string & player::get_id() const
{
    return id;
}

const string & player::get_name() const
{
    return name;
}

const color & player::get_color() const
{
    return c;
}

int player::get_score() const
{
    return score;
}

bool player::is_connected() const
{
    return connected;
}

const buzzer& player::get_buzzer() const
{
    return mybuzzer;
}

bool player::has_buzzed() const
{
    return buzzed;
}

const std::chrono::duration<int, std::milli>& player::get_buzztime() const
{
    return buzztime;
}

GenericValue<UTF8<>> player::buzzed_value() const
{
    if (!buzzed)
        return Value();
    return Value(buzztime.count());
}

void player::store_state(GenericValue<rapidjson::UTF8<>> &root, GenericValue<UTF8<>>::AllocatorType &allocator) const
{
    root.SetObject();
    root.AddMember("id", Value(id.c_str(), id.size()), allocator);
    root.AddMember("name", Value(name.c_str(), name.size()), allocator);
    root.AddMember("color", Value(c.string().c_str(), c.string().size()), allocator);
    root.AddMember("score", score, allocator);
    root.AddMember("buzzed", buzzed, allocator);
    root.AddMember("buzztime", buzztime.count(), allocator);
}
