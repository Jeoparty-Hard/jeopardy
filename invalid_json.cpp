#include "invalid_json.hpp"

invalid_json::invalid_json(std::string what_string)
{
    this->what_string = what_string;
}

const char* invalid_json::what() const noexcept
{
    return what_string.c_str();
}
