#pragma once
#include "chess/Board.h"
#include "Pipe.h"
#include <string>

class Manager {
public:
    Manager(Pipe& pipe, const std::string& boardData);
    void gameLoop();
    void displayBoard() const;
private:
    Pipe& _pipe;
    Board _board;
};
