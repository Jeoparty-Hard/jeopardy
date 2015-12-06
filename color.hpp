#ifndef JEOPARDY_COLOR_H
#define JEOPARDY_COLOR_H

#include <string>

class color
{
private:
    unsigned char vr, vg, vb;
public:
    color(unsigned char r, unsigned char g, unsigned char b);
    color(std::string color);
    unsigned char r() const;
    unsigned char g() const;
    unsigned char b() const;
    std::string string() const;
};

#endif //JEOPARDY_COLOR_H
