#include "jeopardy_round.hpp"

jeopardy_round::jeopardy_round(const rapidjson::GenericValue<rapidjson::UTF8<>> &round, boost::filesystem::path round_path)
{
    this->name = round["name"].GetString();
    auto &categories = round["categories"];
    auto &pointsData = round["points"];

    auto size = categories.Capacity();
    this->categories.resize(size);
    std::vector<unsigned int> points(pointsData.Capacity());

    for (int i = 0;i < size;i++)
    {
        points[i] = pointsData[i].GetInt();
    }

    for (int i = 0;i < size;i++)
    {
        this->categories[i] = category(categories[i], round_path, points);
    }
}

std::string jeopardy_round::get_name() const
{
    return name;
}

const std::vector<category>&jeopardy_round::get_categories() const
{
    return categories;
}
