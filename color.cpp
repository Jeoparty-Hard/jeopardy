#include "color.hpp"

#include <sstream>
#include <iomanip>

using namespace std;

color::color()
    :color(0, 0, 0)
{
    // Nothing to do
}

color::color(unsigned char r, unsigned char g, unsigned char b)
{
    this->vr = r;
    this->vg = g;
    this->vb = b;
    stringstream ss;
    ss << "#" << hex << setfill('0') << setw(2) << (int)vr << setfill('0') << setw(2) << (int)vg << setfill('0') << setw(2) << (int)vb;
    this->colorstr = ss.str();
}

color::color(std::string color)
{
    this->colorstr = color;
    color.erase(0, 1);
    int colorno = stoi(color, 0, 16);
    this->vr = colorno / 256 / 256;
    this->vg = colorno / 256 % 256;
    this->vb = colorno % 256;
}

unsigned char color::r() const
{
    return vr;
}

unsigned char color::g() const
{
    return vg;
}

unsigned char color::b() const
{
    return vb;
}

const std::string & color::string() const
{
    return colorstr;
}
