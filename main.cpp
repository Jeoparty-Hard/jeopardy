#include <iostream>
#include <string>

#include "game.hpp"

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
    game game(port);
    return 0;
}