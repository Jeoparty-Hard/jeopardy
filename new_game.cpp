#include "new_game.hpp"

#include <boost/filesystem.hpp>

#include <valijson/adapters/rapidjson_adapter.hpp>
#include <valijson/utils/file_utils.hpp>
#include <valijson/schema.hpp>
#include <valijson/schema_parser.hpp>
#include <valijson/validator.hpp>

using namespace std;
using namespace boost::filesystem;
using namespace rapidjson;

new_game::new_game(list<player> *players, vector<category> *categories, websocket_server *server)
    : game_state(players, categories, server)
{
    path rounds_dir = "rounds";
    if (!is_directory(rounds_dir))
        return; // TODO Throw exception

    // Initialize json validator
    Document schema_doc;
    string json;
    valijson::utils::loadFile("json-schema/files/round.json", json); // TODO Check if file exists
    schema_doc.Parse(json.c_str()); // TODO Check if schema is valid JSON file

    valijson::Schema schema;
    valijson::SchemaParser parser;
    valijson::adapters::RapidJsonAdapter schemaAdapter(schema_doc);
    parser.populateSchema(schemaAdapter, schema);

    for (directory_iterator it(rounds_dir), end;it != end;it++)
    {
        path current_directory = *it;
        if (!is_directory(rounds_dir))
            continue;
        path json_file = current_directory / "round.json";
        if (!is_regular_file(json_file))
            continue; // TODO Send warning to client

        Document d;
        valijson::utils::loadFile(json_file.string(), json);
        d.Parse(json.c_str()); // TODO Check if file is valid json

        valijson::Validator validator(schema);
        valijson::adapters::RapidJsonAdapter targetAdapter(d);
        valijson::ValidationResults results;

        if (!validator.validate(targetAdapter, &results))
        {
            // TODO Send warning to client
            continue;
        }

        rounds.emplace(current_directory.filename().string(), jeopardy_round(d, current_directory));
    }
}

bool new_game::process_event(const rapidjson::GenericValue<rapidjson::UTF8<>> &event)
{
    // TODO Implement
}

void new_game::current_state(rapidjson::Document &d)
{
    d.SetObject();
    d.AddMember("state", "new", d.GetAllocator());
    Value rounds_value;
    rounds_value.SetObject();
    for (auto &round : rounds)
    {
        rounds_value.AddMember(Value(round.first.c_str(), round.first.size()), Value(round.second.get_name().c_str(), round.second.get_name().size()), d.GetAllocator());
    }
    d.AddMember("rounds", rounds_value, d.GetAllocator());
}
