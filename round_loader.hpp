#ifndef JEOPARDY_ROUND_LOADER_H
#define JEOPARDY_ROUND_LOADER_H

#include <list>
#include <memory>

#include "jeopardy_round.hpp"

namespace round_loader
{
    std::list<jeopardy_round> load_rounds();
    jeopardy_round load_round(const std::string &name);
}

#endif //JEOPARDY_ROUND_LOADER_H
