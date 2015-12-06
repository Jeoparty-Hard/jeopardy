#include "new_game.hpp"

#include <boost/filesystem.hpp>

#include <rapidjson/error/en.h>
#include <valijson/adapters/rapidjson_adapter.hpp>
#include <valijson/utils/file_utils.hpp>
#include <valijson/schema.hpp>
#include <valijson/schema_parser.hpp>
#include <valijson/validator.hpp>

#include "invalid_json.hpp"
#include "jeopardy_exception.hpp"
#include "setup_game.hpp"

using namespace std;
using namespace boost::filesystem;
using namespace rapidjson;

new_game::new_game(list<player> *players, vector<category> *categories, websocket_server *server, unique_ptr<game_state> *next_state)
    : game_state(players, categories, server, next_state)
{
    path rounds_dir = "rounds";
    if (!is_directory(rounds_dir))
        throw jeopardy_exception("'" + rounds_dir.string() + "' is not a directory");

    // Initialize json validator
    path schema_file = path("json-schema") / "files" / "round.json";
    if (!is_regular_file(schema_file))
        throw jeopardy_exception("'" + schema_file.string() + "' is not a regular file");
    Document schema_doc;
    string json;
    valijson::utils::loadFile(schema_file.string(), json);
    schema_doc.Parse(json.c_str());
    if (schema_doc.HasParseError())
        throw invalid_json(valijson::ValidationResults::Error({schema_file.string()}, GetParseError_En(schema_doc.GetParseError())));

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
        d.Parse(json.c_str());
        if (schema_doc.HasParseError())
            continue; // TODO Send warning to client

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

void new_game::initialize()
{
    players.clear();
    categories.clear();
}

bool new_game::process_event(const GenericValue<UTF8<>> &event)
{
    string event_name = event["event"].GetString();
    if (event_name == "select_round")
    {
        string round_id = event["round"].GetString();
        try
        {
            jeopardy_round &round = rounds.at(round_id);
            categories = round.get_categories();
            for (auto &category : categories)
            {
                for (auto &answer : category.get_mutable_answers())
                {
                    answer.load_data();
                }
            }
            next_state.reset(new setup_game(&players, &categories, &server, &next_state));
        }
        catch (out_of_range &)
        {
            throw jeopardy_exception("round '" + round_id + "' doesn't exist");
        }
        return true;
    }
    return false;
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
