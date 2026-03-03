#include "TUI-library/window.hpp"
#include <iostream>
#include <thread>
#include <chrono>

int main(){
    windows::RootWindow janela(Size(100, 100));
    auto f1 = janela.splitHorizontal(50.0);
    auto f2 = f1[0].splitVertical(50.0);
    auto f3 = f1[1].splitHorizontal(50.0); 
    auto f4 = f2[0].splitVertical(50.0);
    auto f5 = f2[1].splitHorizontal(50.0);
    f5[0].setBorderColor(Color::BLUE).setBorderStyle(Style::ROUNDED);
    f3[0].setBorderStyle(Style::ASCII).setBorderColor(Color::RED);
    
    Text m = "Maria\nLuisa!";
    TextList a = {"Maria", "Luisa", "Silva", "Tadaiesky", "Barboza"};
    m.changeColor(Color::YELLOW);
    m.changeCoord(Point(5,5));
    f3[0].addContent(&m);
    f5[0].addContent(&a);

    while (true){
        janela.render();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    return 0;
}