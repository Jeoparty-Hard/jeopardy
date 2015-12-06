#ifndef JEOPARDY_ROUND_CPP_H
#define JEOPARDY_ROUND_CPP_H

#include <string>
#include <vector>

#include <boost/filesystem.hpp>

#include "category.hpp"

class jeopardy_round
{
private:
    std::string id;
    std::string name;
    std::vector<category> categories;
public:
    jeopardy_round(std::string id, const rapidjson::GenericValue<rapidjson::UTF8<>> &round, boost::filesystem::path round_path);
    const std::string & get_id() const;
    const std::string & get_name() const;
    const std::vector<category> & get_categories() const;
};

#endif //JEOPARDY_ROUND_CPP_H
