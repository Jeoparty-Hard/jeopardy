#ifndef JEOPARDY_INVALID_EVENT_H
#define JEOPARDY_INVALID_EVENT_H

#include "invalid_json.hpp"

class invalid_event : public invalid_json
{
public:
    invalid_event();
    virtual ~invalid_event() = default;
};

#endif //JEOPARDY_INVALID_EVENT_H
