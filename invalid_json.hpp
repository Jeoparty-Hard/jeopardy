#ifndef JEOPARDY_INVALID_JSON_H
#define JEOPARDY_INVALID_JSON_H

#include <exception>
#include <string>

class invalid_json : public std::exception
{
private:
    std::string what_string;
public:
    invalid_json(std::string what_string);
    virtual ~invalid_json() = default;
    virtual const char* what() const noexcept;
};

#endif //JEOPARDY_INVALID_JSON_H
