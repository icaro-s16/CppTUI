#ifndef CONTENT_HPP
#define CONTENT_HPP

#include "utils.hpp"
#include "includes.hpp"


enum Color{DEFAULT, WHITE, BLUE, YELLOW, BLACK, GREEN, RED, CYAN};

struct Cell{
    Cell(std::string new_wchar, Color new_color = DEFAULT) : wchar(new_wchar), color(new_color){} 
    
    void changeColor(Color _color){
        color = _color;
    }
    std::string get_wchar(){
        switch(color){
            case BLACK:
                return "\033[30m" + wchar + "\033[0m";
            case RED:
                return "\033[31m" + wchar + "\033[0m";
            case GREEN:
                return "\033[32m" + wchar + "\033[0m";
            case YELLOW:
                return "\033[33m" + wchar + "\033[0m";
            case CYAN:
                return "\033[36m" + wchar + "\033[0m";
            case WHITE:
                return "\033[37m" + wchar + "\033[0m";
            case BLUE:
                return "\033[34m" + wchar + "\033[0m";
            default:
                return wchar;
        }
    }
    // Utilizado apenas para análise de caracteres como \n ou \0
    char get_char(){
        return wchar[0];
    }
private:   
    std::string wchar;
    Color color;
};


class Content{
public:
    Content(Point _coords = Point(1, 1)) : coord(_coords){}
    virtual void renderContent(std::vector<std::vector<Cell>>& buffer, Size actual_size){
        (void)buffer;
        (void)actual_size;
    }
protected:
    Point coord;

};


class Text : public Content{
public:
    Text() : Content() {
        textCells = std::vector<Cell>();
    };
    Text(
        std::string _text,
        Point _point = Point(1, 1),
        Color color = Color::DEFAULT
    ) : Content(_point){
        for(char cell : _text)
            textCells.push_back(Cell(std::string(1, cell), color));
    }
    Text(
        const char* Ctext,
        Point _point = Point(1, 1),
        Color color = Color::DEFAULT
    ) : Content(_point){
        std::string _text = Ctext;
        for(char cell : _text)
            textCells.push_back(Cell(std::string(1, cell), color));
    }
    Text(
        std::initializer_list<const char*> _text,
        Point _point = Point(1, 1),
        Color color = Color::DEFAULT
    ) : Content(_point){
        for(std::string cell : _text)
            textCells.push_back(Cell(cell, color));
    }

    void renderContent(std::vector<std::vector<Cell>>& buffer, Size actual_size) override{
        if (textCells.empty()) return;
        if ( 
            coord.x  > actual_size.columns - 1 || coord.y  > actual_size.lines - 1 ||
            coord.x  < 1 || coord.y  < 1
        ) return;
        if ( actual_size.columns < 3 || actual_size.lines < 3) return;
        for (int lines = coord.y, textIndex = 0; lines < actual_size.lines - 1; lines ++){    
            bool blankSpace = false;
            for(int columns = coord.x; columns < actual_size.columns - 1; columns++){
                if (size_t(textIndex) >= textCells.size()) return;
                blankSpace = (textCells[size_t(textIndex)].get_char() == '\n' ) ? true : false;
                if (blankSpace == true){
                    buffer[size_t(lines)][size_t(columns)] = Cell(" ");
                }else{
                    buffer[size_t(lines)][size_t(columns)] = textCells[size_t(textIndex++)];
                }
            }
            if (size_t(textIndex) < textCells.size() && textCells[size_t(textIndex)].get_char() == '\n') textIndex++;
        }
    }

    void changeColor(Color _color){
        for(Cell& cells: textCells)
            cells.changeColor(_color);
    }

    void changeCoord(Point _coord){
        coord = _coord;
    }

    size_t len(){
        return textCells.size();
    }
    void push(Cell _cell){
        textCells.push_back(_cell);
    }
    void append(Text _textCells){
        for(Cell _cell : _textCells)
            textCells.push_back(_cell);
    }

    std::vector<Cell>::iterator begin(){
        return textCells.begin();
    }

    std::vector<Cell>::iterator end(){
        return textCells.end();
    }

    void operator+=(Cell _cell){
        textCells.push_back(_cell);
    }
    Text operator+(Text other){
        Text new_text = *this;
        for(Cell cells: other)
            new_text += cells;
        return new_text;
    }

    void operator=(std::string _text){
        for(char cell : _text)
            textCells.push_back(Cell(std::string(1, cell)));
    }

    Cell& operator[](size_t index){
        if (index > textCells.size() - 1) err(1, "Trying to acess a invalid index!");
        return textCells[index];
    }

private:
    std::vector<Cell> textCells;

};

class TextList : public Content{
public:
    TextList(
        std::initializer_list<Text> _list
    ){
        for(Text _text: _list)
            textList.push_back(_text);
    }
    TextList(
        std::initializer_list<const char*> _list
    ){
        for (std::string _text: _list)
            textList.push_back(Text(_text));
    }
    void renderContent(std::vector<std::vector<Cell>>& buffer, Size actual_size) override{
        if (textList.empty()) return;

        if ( 
            coord.x  > actual_size.columns - 1 || coord.y  > actual_size.lines - 1 ||
            coord.x  < 1 || coord.y  < 1
        ) return;
        
        if ( actual_size.columns < 3 || actual_size.lines < 3) return;
        
        for (size_t lines = size_t(coord.y), listSize = 0, textSize = 0; lines < size_t(actual_size.lines) - 1; lines++){
            bool blankSpace = false;
            for(size_t columns = size_t(coord.x); columns < size_t(actual_size.columns) - 1; columns++ ){
                
                if (listSize >= textList.size()) return;
                
                else blankSpace = (textSize >= textList[listSize].len() || textList[listSize][textSize].get_char() == '\n' ) ? true : false;

                if (blankSpace)
                    buffer[lines][columns] = Cell(" ");
                else
                    buffer[lines][columns] = textList[listSize][textSize ++ ];

            }
            if (textSize <  textList[listSize].len() && textList[listSize][textSize].get_char() == '\n') textSize++;
            if (textSize >= textList[listSize].len()) {
                textSize = 0;
                listSize++;
            } 
        }
    } 

    void addCounterFormat(std::function<Text(Text, size_t)> _counterFormat){
        for(size_t i = 0; i < textList.size(); i++)
            textList[i] = _counterFormat(textList[i], i);
    }

    void changeCoord(Point _coord){
        coord = _coord;
    }

    void changeColor(Color _color){
        for (Text& _text: textList)
            _text.changeColor(_color);
    }

    size_t len(){
        return textList.size();
    }
    void push(Text _text){
        textList.push_back(_text);
    }

    void pop(){
        if (textList.size() == 1) err(1, "Trying to use a invalid operator!");
        textList.pop_back();
    }

    void append(TextList _textList){
        for(Text _text : _textList)
            textList.push_back(_text);
    }

    std::vector<Text>::iterator begin(){
        return textList.begin();
    }

    std::vector<Text>::iterator end(){
        return textList.end();
    }

    Text& operator[](size_t index){
        if (index > textList.size() - 1) err(1, "Trying to acess a invalid index!");
        return textList[index];
    }
private:
    std::function<std::string(std::string)> counterFormat = nullptr;
    std::vector<Text> textList;

};



#endif