#ifndef JEOPARDY_ANSWER_H
#define JEOPARDY_ANSWER_H

#include <string>
#include <boost/filesystem.hpp>
#include <rapidjson/document.h>

class answer
{
private:
    std::string type;
    std::string data;
public:
    answer(const rapidjson::GenericValue<UTF8<>> &, boost::filesystem::path category_path);
    std::string getType() const;
    std::string getData() const;
};

#endif //JEOPARDY_ANSWER_H
