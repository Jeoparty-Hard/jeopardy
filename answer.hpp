#ifndef JEOPARDY_ANSWER_H
#define JEOPARDY_ANSWER_H

#include <string>
#include <boost/filesystem.hpp>
#include <rapidjson/document.h>

class answer
{
private:
    std::string type;
    unsigned int points;
    std::string data;
public:
    answer() = default;
    answer(const rapidjson::GenericValue<rapidjson::UTF8<>> &, boost::filesystem::path category_path, unsigned int points);
    std::string getType() const;
    std::string getData() const;
};

#endif //JEOPARDY_ANSWER_H
