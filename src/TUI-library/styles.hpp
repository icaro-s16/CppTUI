#ifndef STYLES_HPP
#define STYLES_HPP

#include <string>

enum Style{ROUNDED, LIGHT, HEAVY, DOUBLE, ASCII};

struct BorderStyle {
    std::string TOP_LEFT;
    std::string TOP_RIGHT;
    std::string BOTTOM_LEFT;
    std::string BOTTOM_RIGHT;
    std::string HORIZONTAL;
    std::string VERTICAL;

    BorderStyle(Style new_style = LIGHT) : style(new_style){
        switch (style)
        {
        case ROUNDED:
            TOP_LEFT = "╭";
            TOP_RIGHT = "╮";
            BOTTOM_LEFT = "╰";
            BOTTOM_RIGHT = "╯";
            HORIZONTAL = "─";
            VERTICAL = "│";
            break;
        case HEAVY:
            TOP_LEFT = "┏";
            TOP_RIGHT = "┓";
            BOTTOM_LEFT = "┗";
            BOTTOM_RIGHT = "┛";
            HORIZONTAL = "━";
            VERTICAL = "┃";
            break;    
        case DOUBLE:
            TOP_LEFT = "╔";
            TOP_RIGHT = "╗";
            BOTTOM_LEFT = "╚";
            BOTTOM_RIGHT = "╝";
            HORIZONTAL = "═";
            VERTICAL = "║";
            break;
        case ASCII:
            TOP_LEFT = "+";
            TOP_RIGHT = "+";
            BOTTOM_LEFT = "+";
            BOTTOM_RIGHT = "+";
            HORIZONTAL = "-";
            VERTICAL = "|";
            break;
        default:
            TOP_LEFT = "┌";
            TOP_RIGHT = "┐";
            BOTTOM_LEFT = "└";
            BOTTOM_RIGHT = "┘";
            HORIZONTAL = "─";
            VERTICAL = "│";
            break;
        }
    }
private:
    Style style;
};




#endif