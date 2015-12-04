#include "answer.hpp"

#include <vector>

using namespace std;
using namespace boost::filesystem;
using namespace rapidjson;

answer::answer(const GenericValue<UTF8<>> &answer, path category_path)
{
    this->type = answer["type"].GetString();
    if (type == "text") {
        this->data = answer["data"].GetString();
    }
    else {
        string filename = answer["data"].GetString();
        fstream file(category_path / filename, ios::ate | ios::binary);
        auto size = file.tellg();
        file.seekg(0);
        vector<char> buffer(size);
        file.read(buffer.data(), size);
        file.close();
    }
    this->type = type;
    this->data = data;
}

string getType() const
{
    return type;
}

string getData() const
{
    return data;
}
