#include "category.hpp"

#include "invalid_json.hpp"

#include <valijson/validation_results.hpp>

using namespace std;
using namespace boost::filesystem;
using namespace rapidjson;

const string & category::get_name() const
{
    return name;
}

category::category(const GenericValue<UTF8<>> &category, path round_path, const std::vector<unsigned int> &points)
{
    this->name = category["name"].GetString();
    auto &answers = category["answers"];
    auto size = answers.Capacity();
    if (size != points.size()) {
        valijson::ValidationResults::Error error({round_path.string(), this->name}, "\"The length of the points array and the length of the answers array have to be equal\"");
        throw invalid_json(error);
    }
    this->answers.resize(size);
    path category_path = round_path / category["path"].GetString();
    for (int i = 0;i < size;i++)
    {
        this->answers[i] = answer(answers[i], category_path, points[i]);
    }
}

const std::vector<answer>& category::get_answers() const
{
    return answers;
}

std::vector<answer>& category::get_mutable_answers()
{
    return answers;
}
