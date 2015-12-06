#ifndef JEOPARDY_JEOPARDY_EXCEPTION_H
#define JEOPARDY_JEOPARDY_EXCEPTION_H

#include <exception>
#include <string>

class jeopardy_exception : public std::exception
{
protected:
    std::string what_data;
public:
    jeopardy_exception(std::string what);
    virtual ~jeopardy_exception() = default;
    virtual const char* what() const noexcept;
};

#endif //JEOPARDY_JEOPARDY_EXCEPTION_H
