#include "answer.hpp"

#include <vector>
#include <boost/filesystem/fstream.hpp>

#include "base64.hpp"

using namespace std;
using namespace boost::filesystem;
using namespace rapidjson;

answer::answer(const GenericValue<UTF8<>> &answer, path category_path, unsigned int points)
{
    this->type = answer["type"].GetString();
    this->data = answer["data"].GetString();
    this->points = points;
    this->data_loaded = false;
    this->category_path = category_path;
}

string answer::getType() const
{
    return type;
}

string answer::getData() const
{
    return data;
}

void answer::load_data()
{
    if (data_loaded)
        return;
    if (type != "text") {
        boost::filesystem::ifstream file(category_path / data, ios::ate | ios::binary);
        auto size = file.tellg();
        file.seekg(0);
        vector<char> buffer(size);
        file.read(buffer.data(), size);
        file.close();
        data = base64encode(buffer);
    }
    data_loaded = true;
}