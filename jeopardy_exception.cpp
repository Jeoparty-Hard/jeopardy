#include "jeopardy_exception.hpp"

jeopardy_exception::jeopardy_exception(std::string what)
{
    this->what_data = what;
}

const char* jeopardy_exception::what() const noexcept
{
    return what_data.c_str();
}
