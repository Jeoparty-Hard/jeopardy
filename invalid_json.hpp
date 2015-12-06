#ifndef JEOPARDY_INVALID_JSON_H
#define JEOPARDY_INVALID_JSON_H

#include <exception>
#include <vector>

#include <valijson/validation_results.hpp>

class invalid_json : public std::exception
{
protected:
    std::vector<valijson::ValidationResults::Error> errors;
public:
    invalid_json(valijson::ValidationResults results);
    invalid_json(valijson::ValidationResults::Error error);
    virtual ~invalid_json() = default;
    virtual const char* what() const noexcept;
    const std::vector<valijson::ValidationResults::Error> & get_errors() const;
};

#endif //JEOPARDY_INVALID_JSON_H
