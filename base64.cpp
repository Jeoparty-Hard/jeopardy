#include "base64.hpp"

using namespace std;

static const char* base64chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

string base64encode(const vector<char> &data)
{
    char byte0;
    char byte1;
    char byte2;
    unsigned char char0;
    unsigned char char1;
    unsigned char char2;
    unsigned char char3;
    int fillbytes = 0;
    string result = "";
    for (size_t i = 0;i < data.size();i += 3)
    {
        byte0 = data[i];
        if (i + 1 < data.size())
        {
            byte1 = data[i + 1];
            if (i + 2 < data.size())
            {
                byte2 = data[i + 2];
            }
            else
            {
                byte2 = 0;
                fillbytes = 1;
            }
        }
        else
        {
            byte1 = 0;
            byte2 = 0;
            fillbytes = 2;
        }
        char0 = byte0 >> 2;
        char1 = (byte0 & 0b00000011) << 4 | (byte1 & 0b11110000) >> 4;
        char2 = (byte1 & 0b00001111) << 2 | (byte2 & 0b11000000) >> 6;
        char3 = byte2 & 0b00111111;
        result += base64chars[char0];
        result += base64chars[char1];
        result += base64chars[char2];
        result += base64chars[char3];
    }
    for (int i = 0;i < fillbytes;i++)
    {
        result[result.size() - i - 1] = '=';
    }
    return result;
}