#include "category.hpp"

#include "invalid_json.hpp"

using namespace std;
using namespace boost::filesystem;
using namespace rapidjson;

category::category(const GenericValue<UTF8<>> &category, path round_path, const std::vector<unsigned int> &points)
{
    this->name = category["name"].GetString();
    auto &answers = category["answers"];
    auto size = answers.Capacity();
    if (size != points.size())
        throw invalid_json("The length of the points array and the length of the answers array have to be equal");
    this->answers.resize(size);
    path category_path = round_path / category["path"].GetString();
    for (int i = 0;i < size;i++)
    {
        this->answers[i] = answer(answers[i], category_path, points[i]);
    }
}

std::string category::get_name() const
{
    return name;
}

const std::vector<answer>& category::get_answers() const
{
    return answers;
}
