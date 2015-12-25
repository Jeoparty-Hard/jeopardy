#include "jeopardy_round.hpp"

using namespace std;

jeopardy_round::jeopardy_round(string id, const rapidjson::GenericValue<rapidjson::UTF8<>> &round, boost::filesystem::path round_path)
{
    this->id = id;
    this->name = round["name"].GetString();
    auto &categories = round["categories"];
    auto &pointsData = round["points"];

    auto size = categories.Capacity();
    this->categories.resize(size);
    std::vector<unsigned int> points(pointsData.Capacity());

    for (size_t i = 0;i < points.size();i++)
    {
        points[i] = pointsData[i].GetInt();
    }

    for (size_t i = 0;i < size;i++)
    {
        this->categories[i] = category(categories[i], round_path, points, i);
    }
}

const string & jeopardy_round::get_id() const
{
    return id;
}

const string & jeopardy_round::get_name() const
{
    return name;
}

const vector<category>&jeopardy_round::get_categories() const
{
    return categories;
}
