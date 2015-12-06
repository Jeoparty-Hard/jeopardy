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
    bool data_loaded;
    boost::filesystem::path category_path;
public:
    answer() = default;
    answer(const rapidjson::GenericValue<rapidjson::UTF8<>> &, boost::filesystem::path category_path, unsigned int points);
    std::string get_type() const;
    std::string get_data() const;
    unsigned int get_points() const;
    void load_data();
};

#endif //JEOPARDY_ANSWER_H
