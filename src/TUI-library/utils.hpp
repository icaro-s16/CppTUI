#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>


struct Size{
    unsigned short lines;
    unsigned short columns;
    Size(){}
    Size(unsigned short new_lines, unsigned short new_columns) : lines(new_lines), columns(new_columns){}
    bool operator==(Size other){
        return this->columns == other.columns && this->lines == other.lines;
    }
    bool operator!=(Size other){
        return this->columns != other.columns && this->lines != other.lines;
    }
    void operator-=(unsigned short i){
        lines -= i;
        columns -= i;
    }
    Size operator-(unsigned short i){
        lines  -= i;
        columns -= i;
        return *this;
    }
};

struct Point{
    unsigned short x;
    unsigned short y;
    Point(unsigned short n_x, unsigned short n_y) : x(n_x), y(n_y) {}
};



#endif