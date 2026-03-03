#include <iostream>
#include <termios.h>
#include <unistd.h>


// Função para configurar o terminal em modo raw
void setRawMode(bool enable) {
    static struct termios oldt, newt;
    if (enable) {
        tcgetattr(STDIN_FILENO, &oldt); // Pega configurações atuais
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO); // Desabilita canônico e echo
        tcsetattr(STDIN_FILENO, TCSANOW, &newt); // Aplica as novas
    } else {
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt); // Restaura o original
    }
}

int main() {
    char ch;
    std::cout << "Pressione teclas (q para sair):" << std::endl;
    std::string t = "";
    setRawMode(true); // Ativa o modo raw

    while (true) {
        if (read(STDIN_FILENO, &ch, 1) == 1) { // Lê 1 caractere
            if (ch == 'q') break;
            else t += ch;
            std::cout << "Tecla pressionada: " << ch << " (ASCII: " << (int)ch << ")" << std::endl;
            
        }
        std::cout << t << std::endl;
    }

    setRawMode(false); // Restaura o modo padrão antes de sair
    return 0;
}

