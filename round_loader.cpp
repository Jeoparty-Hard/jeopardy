#include "round_loader.hpp"

#include <iostream>
#include <string>

#include <boost/filesystem.hpp>
#include <rapidjson/document.h>
#include <rapidjson/error/en.h>
#include <valijson/utils/file_utils.hpp>
#include <valijson/schema_parser.hpp>
#include <valijson/adapters/rapidjson_adapter.hpp>
#include <valijson/validator.hpp>

#include "invalid_json.hpp"
#include "jeopardy_exception.hpp"

using namespace std;
using namespace boost::filesystem;
using namespace rapidjson;
using namespace valijson;
using namespace valijson::adapters;

static Schema initialize_schema()
{
    path schema_file = path("json-schema") / "files" / "round.json";
    if (!is_regular_file(schema_file))
        throw jeopardy_exception("'" + schema_file.string() + "' is not a regular file");
    Document schema_doc;
    string json;
    valijson::utils::loadFile(schema_file.string(), json);
    schema_doc.Parse(json.c_str());
    if (schema_doc.HasParseError())
        throw invalid_json(valijson::ValidationResults::Error({schema_file.string()}, GetParseError_En(schema_doc.GetParseError())));

    Schema schema;
    SchemaParser parser;
    RapidJsonAdapter schemaAdapter(schema_doc);
    parser.populateSchema(schemaAdapter, schema);
    return schema;
}

static jeopardy_round load_round(const path &round_directory, Validator &validator)
{
    if (!is_directory(round_directory))
        throw jeopardy_exception("'" + round_directory.string() + "' is not a directory");

    path json_file = round_directory / "round.json";
    if (!is_regular_file(json_file))
        throw jeopardy_exception("'" + round_directory.string() + "' is not a file");

    Document d;
    string json;
    valijson::utils::loadFile(json_file.string(), json);
    d.Parse(json.c_str());
    if (d.HasParseError())
        throw invalid_json("Error while parsing '" + json_file.string() + "': " + GetParseError_En(d.GetParseError()));

    adapters::RapidJsonAdapter targetAdapter(d);
    ValidationResults results;
    if (!validator.validate(targetAdapter, &results))
        throw invalid_json(results);

    return jeopardy_round(jeopardy_round(round_directory.filename().string(), d, round_directory));
}

list<jeopardy_round> round_loader::load_rounds()
{
    path rounds_dir = "rounds";
    if (!is_directory(rounds_dir))
        throw jeopardy_exception("'" + rounds_dir.string() + "' is not a directory");
    Validator validator(initialize_schema());
    list<jeopardy_round> rounds;
    for (directory_iterator it(rounds_dir), end;it != end;it++)
    {
        path current_directory = *it;
        if (!is_directory(rounds_dir))
        {
            cerr << current_directory << " is not a directory" << endl;
            continue;
        }
        try
        {
            rounds.push_back(::load_round(current_directory, validator));
        }
        catch (jeopardy_exception &e)
        {
            cerr << e.what() << endl;
        }
    }
    return rounds;
}

jeopardy_round round_loader::load_round(const std::string &name)
{
    path rounds_dir = "rounds";
    if (!is_directory(rounds_dir))
        throw jeopardy_exception("'" + rounds_dir.string() + "' is not a directory");
    Validator validator(initialize_schema());
    path round_directory = rounds_dir / name;
    return ::load_round(round_directory, validator);
}
