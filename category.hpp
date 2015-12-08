#ifndef JEOPARDY_CATEGORY_H
#define JEOPARDY_CATEGORY_H

#include <string>
#include <vector>

#include <boost/filesystem.hpp>
#include <rapidjson/document.h>

#include "answer.hpp"

class category
{
private:
    std::string name;
    std::vector<answer> answers;
public:
    category() = default;
    category(const rapidjson::GenericValue<rapidjson::UTF8<>> &, boost::filesystem::path round_path, const std::vector<unsigned int> &points);
    const std::string & get_name() const;
    const std::vector<answer> & get_answers() const;
    std::vector<answer> & get_mutable_answers();
};

#endif //JEOPARDY_CATEGORY_H
