#include "Manager.h"
#include "chess/MoveException.h"
#include <cstring>
#include <iostream>

Manager::Manager(Pipe& pipe, const std::string& boardData) : _pipe(pipe), _board(boardData) {}
void Manager::displayBoard() const { std::cout << _board; }

void Manager::gameLoop() {
    std::string initial=_board.toString();
    _pipe.sendMessageToGraphics(initial.c_str());
    displayBoard();
    for (std::string message=_pipe.getMessageFromGraphics(); message!="quit"; message=_pipe.getMessageFromGraphics()) {
        Status response=MOVE_INVALID_OUT_OF_BOUNDS;
        try {
            if (message.size()!=4) throw MoveException(MOVE_INVALID_OUT_OF_BOUNDS);
            response=_board.movePiece(message.substr(0,2),message.substr(2,2));
        } catch (const MoveException& error) {
            response=error.getErrorCode();
        } catch (const std::exception&) {
            response=MOVE_INVALID_ILLEGAL_PIECE_MOVE;
        }
        const std::string code=std::to_string(static_cast<int>(response));
        _pipe.sendMessageToGraphics(code.c_str());
        displayBoard();
    }
}
