#ifndef JEOPARDY_DATA_LOADER_H
#define JEOPARDY_DATA_LOADER_H

#include <list>
#include <memory>

#include "jeopardy_round.hpp"

namespace data_loader
{
    std::list<jeopardy_round> load_rounds();
    jeopardy_round load_round(const std::string &name);
}

#endif //JEOPARDY_DATA_LOADER_H
