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
    this->points = points;
    if (type == "text") {
        this->data = answer["data"].GetString();
    }
    else {
        string filename = answer["data"].GetString();
        boost::filesystem::ifstream file(category_path / filename, ios::ate | ios::binary);
        auto size = file.tellg();
        file.seekg(0);
        vector<char> buffer(size);
        file.read(buffer.data(), size);
        file.close();
        this->data = base64encode(buffer);
    }
    this->type = type;
}

string answer::getType() const
{
    return type;
}

string answer::getData() const
{
    return data;
}
