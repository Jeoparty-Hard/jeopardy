
#include <iostream>
#include <string>

#include "game.hpp"
#include "invalid_json.hpp"

using namespace std;

int main(int argc, char **argv)
{
    int port = 4242;
    if (argc >= 2)
    {
        port = stoi(string(argv[1]));
    }
    else
    {
        cout << "No port specified. Defaulting to " << port << endl;
    }
    try
    {
        game game(port);
    }
    catch (invalid_json &e)
    {
        cerr << "Cannot validate json" << endl;
        for (auto &error : e.get_errors())
        {
            cerr << error.description << endl;
            for (auto &context : error.context)
            {
                cerr << "\t" << context << endl;
            }
        }
    }
    return 0;
}