#include "answer.hpp"

#include <vector>
#include <boost/filesystem/fstream.hpp>

#include "base64.hpp"

using namespace std;
using namespace boost::filesystem;
using namespace rapidjson;

answer::answer(const GenericValue<UTF8<>> &answer, path category_path, unsigned int points, unsigned int col, unsigned int row)
{
    this->col = col;
    this->row = row;
    this->type = answer["type"].GetString();
    this->data = answer["data"].GetString();
    this->points = points;
    this->data_loaded = false;
    this->category_path = category_path;
    this->has_winner = false;
    this->winner = nullptr;
}

void answer::set_winner(player *winner)
{
    this->has_winner = true;
    this->winner = winner;
}

const string & answer::get_type() const
{
    return type;
}

const string & answer::get_data() const
{
    return data;
}

unsigned int answer::get_points() const
{
    return points;
}

unsigned int answer::get_col() const
{
    return col;
}

unsigned int answer::get_row() const
{
    return row;
}

rapidjson::GenericValue<rapidjson::UTF8<>> answer::winner_value() const
{
    if (!has_winner)
        return Value();
    if (winner == nullptr)
        return Value(false);
    return Value(winner->get_id().c_str(), winner->get_id().size());
}

void answer::load_data()
{
    if (data_loaded)
        return;
    if (type != "text") {
        boost::filesystem::ifstream file(category_path / data, ios::ate | ios::binary);
        auto size = file.tellg();
        file.seekg(0);
        vector<char> buffer(size);
        file.read(buffer.data(), size);
        file.close();
        data = base64encode(buffer);
    }
    data_loaded = true;
}
