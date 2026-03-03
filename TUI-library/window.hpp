#ifndef WINDOW_HPP
#define WINDOW_HPP

#include "includes.hpp"
#include "styles.hpp"
#include "utils.hpp"
#include "content.hpp"


namespace windows{
    namespace details{
        winsize TERMINAL_SIZE;

        void handleResize(int signal){
            (void)signal;
            std::system("clear");
            if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &TERMINAL_SIZE) < 0) err(8, "dev/tty");    
        }

        enum Orientation{HORIZONTAL, VERTICAL};

        class Window {
        public:
            Window(bool n_max_size_is_on = false){
                if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &TERMINAL_SIZE) < 0) err(8, "dev/tty");
                max_size_is_on = n_max_size_is_on;
                actual_size.lines = TERMINAL_SIZE.ws_row;
                actual_size.columns = TERMINAL_SIZE.ws_col;
                max_size.lines = actual_size.lines;
                max_size.columns = actual_size.columns;
                content = std::vector<Content*>();
                signal(SIGWINCH, handleResize);
            }
            Window(
                Size new_max_size
            ) : max_size(new_max_size){
                if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &TERMINAL_SIZE) < 0) err(8, "dev/tty");
                max_size_is_on = true;
                actual_size.lines = (TERMINAL_SIZE.ws_row >= max_size.lines) ? max_size.lines : TERMINAL_SIZE.ws_row;
                actual_size.columns = (TERMINAL_SIZE.ws_col >= max_size.columns) ? max_size.columns : TERMINAL_SIZE.ws_col;
                content = std::vector<Content*>();
                signal(SIGWINCH, handleResize);
            }

  
        
            void addContent(Content* _content){
                content.push_back(_content);
            }


            
            void renderContent(){
                if (content.size() == 0) return;
                for(Content* _content: content)
                    _content->renderContent(cells, actual_size);
            }

            void putChar(Point coord, Cell cell){
                cells[coord.x][coord.y] = cell;
            }

            Window& setBorderStyle(Style new_style){
                style = new_style;
                if (border) box();
                return *this;
            }

            Window& setBorderColor(Color new_color){
                color = new_color;
                if (border) box();
                return *this;
            }
            Window& NoBorder(){
                border = false;
                return *this;
            }


            Size getSize(){
                return actual_size;
            }

            double getPercent(){
                return percent;
            }
        protected:
            std::vector<std::vector<Cell>> cells; // Buffer de desenho
            std::vector<Content*> content;
            Size actual_size; 
            double percent = 0; // Porcentagem de divisão dos filhos
            Size max_size;
            bool max_size_is_on = false; 
            
            Orientation split_childrens_type; // Orientação de divisão das janelas
            
            Style style = Style::LIGHT;
            Color color = Color::DEFAULT;
            bool border = true;

            void moveCursorTo(Point point){
                std::cout << "\033[" + std::to_string(point.y) + ";" + std::to_string(point.x) + "H" << std::flush;
            }


            void box(){
                BorderStyle border_style(style);
                cells[0][0] = Cell(border_style.TOP_LEFT, color);
                cells[0][actual_size.columns - 1] = Cell(border_style.TOP_RIGHT, color); 
                cells[actual_size.lines - 1][0] = Cell(border_style.BOTTOM_LEFT, color);
                cells[actual_size.lines - 1][actual_size.columns - 1] = Cell(border_style.BOTTOM_RIGHT, color);
                for (unsigned short i = 1; i < actual_size.columns - 1; i++){
                    cells[0][i] = Cell(border_style.HORIZONTAL, color);
                    cells[actual_size.lines - 1][i] = Cell(border_style.HORIZONTAL, color);
                }
                for (unsigned short i = 1; i < actual_size.lines - 1; i ++){
                    cells[i][0] = Cell(border_style.VERTICAL, color);
                    cells[i][actual_size.columns - 1] = Cell(border_style.VERTICAL, color);
                }
            }


        };
    }

    /*
    Objetivo: Essas janelas são criadas para dividir sua janela pai e sobrescrevê-la, a partir de uma orientação. 
    Elas não devem ser criados de forma aleatória, mas sim a partir do método split da janela root correspondente.
    
    Divisão:  A porcentagem passada como parâmetro corresponde a porcentagem da janela filho índice 0.
    A porcentagem restante é passada para a janela filho índice 1.
    */
    class ChildrenWindow : public details::Window{
    public:
        ChildrenWindow(bool new_max_size_is_on = false) : Window(new_max_size_is_on) {
            constructCells();
        }
        ChildrenWindow(Size new_max_size) : Window(new_max_size){
            constructCells();
        }
        ~ChildrenWindow(){
            if (childrens.empty())
                return;
            deleteChildrens(childrens[0]);
            deleteChildrens(childrens[1]);
        }
        class Holder{
        private:
            struct Wrapper{
                ChildrenWindow& ref;
            };
        public:
            std::vector<Wrapper*> ref_vec;
            ~Holder(){
                for(Wrapper* i: ref_vec)
                    delete i;
            }
            void push_back(ChildrenWindow* x){
                ref_vec.push_back(new Wrapper{*x});
            }
            ChildrenWindow& operator[](size_t i){
                return ref_vec[i]->ref;
            }
        };

        /*
        Objetivo: Esse método dividi a janela atual em uma orientação vertical e retorna os correspondentes filhos, ao qual a porcentagem passada
        corresponde com a porcentagem de ocupação do filho 1 e o restante com a porcentagem de ocupação do filho 2.
        
        Atenção: A janela pai possui sua borda apagada, junto com suas personalizações e conteúdos internos.
        */
        Holder splitVertical(double n_percent){
            split_childrens_type = details::Orientation::VERTICAL;
            ChildrenWindow* children1 = new ChildrenWindow(true);
            ChildrenWindow* children2 = new ChildrenWindow(true);
            childrens.push_back(children1);
            childrens.push_back(children2);
            childrens[0]->split_childrens_type = split_childrens_type;
            childrens[1]->split_childrens_type = split_childrens_type;
            calculateSplitChildrenSize(n_percent);
            NoBorder();
            Holder holder;
            holder.push_back(children1);
            holder.push_back(children2);
            return  holder;
        }
        /*
        Objetivo: Esse método dividi a janela atual em uma orientação Horizontal e retorna os correspondentes filhos, ao qual a porcentagem passada
        corresponde com a porcentagem de ocupação do filho 1 e o restante com a porcentagem de ocupação do filho 2.
        
        Atenção: A janela pai possui sua borda apagada, junto com suas personalizações e conteúdos internos.
        */
        Holder splitHorizontal(double n_percent){
            split_childrens_type = details::Orientation::HORIZONTAL;
            ChildrenWindow* children1 = new ChildrenWindow(true);
            ChildrenWindow* children2 = new ChildrenWindow(true);
            childrens.push_back(children1);
            childrens.push_back(children2);
            childrens[0]->split_childrens_type = split_childrens_type;
            childrens[1]->split_childrens_type = split_childrens_type;
            calculateSplitChildrenSize(n_percent);
            NoBorder();
            Holder holder;
            holder.push_back(children1);
            holder.push_back(children2);
            return holder;
        }

        void renderTo(ChildrenWindow& block, Point p){
            if (block.actual_size.lines < actual_size.lines || block.actual_size.columns < actual_size.columns) err(1, "invalid block");
            renderChildrensTo();
            renderContent();
            for (unsigned short i = 0; i < actual_size.lines; i++){
                for (unsigned short j = 0; j < actual_size.columns; j++){
                    block.cells[p.y + i][p.x + j] = cells[i][j];
                }
            }
        }  

        void renderChildrensTo(){
            if (childrens.empty()) return;
            if (split_childrens_type == details::Orientation::HORIZONTAL){
                childrens[0]->renderTo(dynamic_cast<ChildrenWindow&>(*this), Point(0, 0));
                childrens[1]->renderTo(dynamic_cast<ChildrenWindow&>(*this), Point(0, childrens[0]->actual_size.lines ));
            }else{
                childrens[0]->renderTo(dynamic_cast<ChildrenWindow&>(*this), Point(0, 0));  
                childrens[1]->renderTo(dynamic_cast<ChildrenWindow&>(*this), Point(childrens[0]->actual_size.columns, 0));
            }
        }

        void updateChildrenSize(ChildrenWindow& father){
            bool constructor_flag = false;
            if ((father.actual_size.columns < max_size.columns || father.actual_size.lines < max_size.lines) || 
            ( father.actual_size.columns != actual_size.columns || father.actual_size.lines != actual_size.lines)) {
                constructor_flag = true;
            }
            actual_size.lines = (father.actual_size.lines >= max_size.lines) ? max_size.lines : father.actual_size.lines;
            actual_size.columns = (father.actual_size.columns >= max_size.columns) ? max_size.columns : father.actual_size.columns;
            if (constructor_flag) constructCells();
            if (!childrens.empty()){
                calculateSplitChildrenSize(childrens[0]->percent);
                childrens[0]->updateChildrenSize(*this);
                childrens[1]->updateChildrenSize(*this);
            }
        }

    protected:
        std::vector<ChildrenWindow*> childrens = std::vector<ChildrenWindow*>();

        void calculateSplitChildrenSize(double n_percent){
            if (percent > 100.0 || percent < 0.0) err(1, "Invalid size for a children");
            Size children_1_size;
            Size children_2_size;
            if (split_childrens_type == details::Orientation::HORIZONTAL){
                children_1_size = Size( u_short(double(actual_size.lines) * (n_percent / 100.0)), actual_size.columns);
                children_2_size = Size(actual_size.lines  -  children_1_size.lines , actual_size.columns);
            }else{
                children_1_size = Size(actual_size.lines, u_short(double(actual_size.columns) * (n_percent / 100.0)));
                children_2_size = Size(actual_size.lines , actual_size.columns - children_1_size.columns);
            }
            childrens[0]->percent = n_percent;
            childrens[1]->percent = 100.0 - n_percent;
            childrens[0]->max_size = children_1_size;
            childrens[1]->max_size = children_2_size;
        }

        void constructCells(){
            cells = std::vector<std::vector<Cell>>(actual_size.lines, std::vector<Cell>(actual_size.columns, Cell(" ")));
            if (border && (actual_size.columns != 0 && actual_size.lines != 0)) box();
            if (!childrens.empty()){
                childrens[0]->constructCells();
                childrens[1]->constructCells();
            }
        }
    private:
        void deleteChildrens(ChildrenWindow* father){
            if(father->childrens.empty()){
                delete father;
                return;
            }
            deleteChildrens(father->childrens[0]);
            deleteChildrens(father->childrens[1]);
        }
    };



    /*
    Objetivo: Essa classe corresponde ao buffer de renderização principal, todos os conteúdos/filhos usam ela de base!
    Apenas ela pode renderizar na saída padrão (stdout) o conteúdo armazenado no seu buffer. 
    */
    class RootWindow : public ChildrenWindow{
    public:
        using ChildrenWindow::ChildrenWindow;
        ~RootWindow(){
            if (!childrens.empty()){
                delete childrens[0];
                delete childrens[1];
            }
        }
        void updateRootSize(){
            bool constructor_flag = false;
            if ((details::TERMINAL_SIZE.ws_col < max_size.columns || details::TERMINAL_SIZE.ws_row < max_size.lines) || 
            ( details::TERMINAL_SIZE.ws_col != actual_size.columns || details::TERMINAL_SIZE.ws_row != actual_size.lines)) {
                constructor_flag = true;
            }
            if(max_size_is_on){
                actual_size.lines = (details::TERMINAL_SIZE.ws_row >= max_size.lines) ? max_size.lines : details::TERMINAL_SIZE.ws_row;
                actual_size.columns = (details::TERMINAL_SIZE.ws_col >= max_size.columns) ? max_size.columns : details::TERMINAL_SIZE.ws_col;
            }else{
                actual_size.lines = details::TERMINAL_SIZE.ws_row;
                actual_size.columns = details::TERMINAL_SIZE.ws_col;
                max_size.lines = actual_size.lines;
                max_size.columns = actual_size.columns;
            }
            if (constructor_flag) constructCells();

            if (!childrens.empty()){
                calculateSplitChildrenSize(childrens[0]->getPercent());
                childrens[0]->updateChildrenSize(dynamic_cast<ChildrenWindow&>(*this));
                childrens[1]->updateChildrenSize(dynamic_cast<ChildrenWindow&>(*this));
            }
        }
        void render(){
            updateRootSize();
            renderChildrensTo();
            moveCursorTo(Point(1,1));
            unsigned short current_terminal_line = 1; 
            for (unsigned short i = 0; i < actual_size.lines; i ++){
                std::string line = "";
                for(unsigned short j = 0; j < actual_size.columns; j ++){
                    line += cells[i][j].get_wchar();
                }
                moveCursorTo(Point(1, current_terminal_line++));
                std::cout << "\033[2K" << line << std::flush;
                line = "";
            }
        }

    };
}

#endif