#ifndef JEOPARDY_DATA_LOADER_H
#define JEOPARDY_DATA_LOADER_H

#include <list>
#include <memory>
#include <valijson/validator.hpp>

#include "jeopardy_round.hpp"
#include "mediator/device_type.hpp"

namespace data_loader
{
    rapidjson::Document load_validated_document(const boost::filesystem::path &json_file, valijson::Validator &);
    std::list<jeopardy_round> load_rounds();
    jeopardy_round load_round(const std::string &name);
    std::list<std::pair<std::string, device_type>> load_default_devices();
}

#endif //JEOPARDY_DATA_LOADER_H
