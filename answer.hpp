#ifndef JEOPARDY_ANSWER_H
#define JEOPARDY_ANSWER_H

#include <memory>
#include <string>
#include <boost/filesystem.hpp>
#include <rapidjson/document.h>

#include "player.hpp"

class answer
{
private:
    unsigned int col, row;
    std::string type;
    unsigned int points;
    std::string data;
    bool data_loaded;
    boost::filesystem::path category_path;
    bool has_winner;
    player *winner;
public:
    answer() = default;
    answer(const rapidjson::GenericValue<rapidjson::UTF8<>> &, boost::filesystem::path category_path, unsigned int points, unsigned int col, unsigned int row);
    bool is_won() const;
    void set_winner(player *winner);
    const std::string & get_type() const;
    const std::string & get_data() const;
    unsigned int get_points() const;
    unsigned int get_col() const;
    unsigned int get_row() const;
    rapidjson::GenericValue<rapidjson::UTF8<>> winner_value(rapidjson::GenericValue<rapidjson::UTF8<>>::AllocatorType &) const;
    void load_data();
};

#endif //JEOPARDY_ANSWER_H
