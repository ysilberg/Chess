#include "Board.h"
#include "Bishop.h"
#include "King.h"
#include "Knight.h"
#include "Pwn.h"
#include "Queen.h"
#include "Rook.h"
#include <cctype>
#include <cstdlib>
#include <ostream>
#include <stdexcept>

namespace {
std::unique_ptr<Piece> makePiece(char symbol, const std::string& position) {
    const char color = std::isupper(static_cast<unsigned char>(symbol)) ? WHITE : BLACK;
    switch (std::tolower(static_cast<unsigned char>(symbol))) {
        case 'p': return std::make_unique<Pwn>(color, position);
        case 'r': return std::make_unique<Rook>(color, position);
        case 'n': return std::make_unique<Knight>(color, position);
        case 'b': return std::make_unique<Bishop>(color, position);
        case 'q': return std::make_unique<Queen>(color, position);
        case 'k': return std::make_unique<King>(color, position);
        default: throw std::invalid_argument("Invalid piece character in board data.");
    }
}
char symbolFor(const Piece& piece) {
    const std::string type = piece.getType();
    char symbol = type == "Knight" ? 'n' : static_cast<char>(std::tolower(type[0]));
    return piece.getColor() == WHITE ? static_cast<char>(std::toupper(symbol)) : symbol;
}
}

Board::Board(const std::string& boardData) {
    if (boardData.size() != CHESS_BOARD_SIZE + 1 || (boardData[64] != '0' && boardData[64] != '1'))
        throw std::invalid_argument("Board data must contain 64 squares followed by turn 0 or 1.");
    _whiteTurn = boardData[64] == '0';
    for (int inputRow=0; inputRow<CHESS_SIZE; ++inputRow) {
        const int row = CHESS_SIZE - 1 - inputRow;
        for (int col=0; col<CHESS_SIZE; ++col) {
            const char value = boardData[inputRow*CHESS_SIZE+col];
            if (value != '#') {
                const std::string position{static_cast<char>('a'+col), static_cast<char>('1'+row)};
                _board[row][col] = makePiece(value, position);
            }
        }
    }
}

std::pair<int,int> Board::indices(const std::string& position) {
    if (position.size()!=2 || position[0]<'a' || position[0]>'h' || position[1]<'1' || position[1]>'8')
        throw MoveException(MOVE_INVALID_OUT_OF_BOUNDS);
    return {position[1]-'1', position[0]-'a'};
}
const Piece* Board::getSymbol(const std::string& position) const { const auto [r,c]=indices(position); return _board[r][c].get(); }
Piece* Board::getSymbol(const std::string& position) { const auto [r,c]=indices(position); return _board[r][c].get(); }

std::string Board::toString() const {
    std::string result; result.reserve(65);
    for (int row=7; row>=0; --row) for (int col=0; col<8; ++col)
        result += _board[row][col] ? symbolFor(*_board[row][col]) : '#';
    result += _whiteTurn ? '0' : '1';
    return result;
}

bool Board::isPathClear(int fr, int fc, int tr, int tc) const {
    const int dr=(tr>fr)-(tr<fr), dc=(tc>fc)-(tc<fc);
    for (int r=fr+dr,c=fc+dc; r!=tr || c!=tc; r+=dr,c+=dc) if (_board[r][c]) return false;
    return true;
}

bool Board::isPseudoLegal(const Piece& piece, int tr, int tc, bool capture) const {
    const auto [fr,fc]=indices(piece.getPosition());
    const int dr=tr-fr, dc=tc-fc;
    if (piece.getType()=="Pwn") {
        const int direction=piece.getColor()==WHITE?1:-1;
        if (capture) return std::abs(dc)==1 && dr==direction;
        if (dc!=0) return false;
        if (dr==direction) return true;
        const int home=piece.getColor()==WHITE?1:6;
        return fr==home && dr==2*direction && !_board[fr+direction][fc];
    }
    const std::string to{static_cast<char>('a'+tc),static_cast<char>('1'+tr)};
    if (!piece.canMove(to)) return false;
    const std::string type=piece.getType();
    return (type=="Rook"||type=="Bishop"||type=="Queen") ? isPathClear(fr,fc,tr,tc) : true;
}

bool Board::isSquareAttacked(int row, int col, char byColor) const {
    for (int r=0;r<8;++r) for (int c=0;c<8;++c) {
        const Piece* piece=_board[r][c].get();
        if (piece && piece->getColor()==byColor && isPseudoLegal(*piece,row,col,true)) return true;
    }
    return false;
}

bool Board::isCheck(char color) const {
    for (int r=0;r<8;++r) for (int c=0;c<8;++c) {
        const Piece* piece=_board[r][c].get();
        if (piece && piece->getColor()==color && piece->getType()=="King")
            return isSquareAttacked(r,c,color==WHITE?BLACK:WHITE);
    }
    throw std::logic_error("Board does not contain the requested king.");
}

Status Board::movePiece(const std::string& from, const std::string& to) {
    const auto [fr,fc]=indices(from); const auto [tr,tc]=indices(to);
    if (fr==tr&&fc==tc) throw MoveException(MOVE_INVALID_IDENTICAL_SQUARES);
    Piece* piece=_board[fr][fc].get();
    if (!piece) throw MoveException(MOVE_INVALID_SOURCE_EMPTY);
    const char movingColor=_whiteTurn?WHITE:BLACK;
    if (piece->getColor()!=movingColor) throw MoveException(MOVE_INVALID_TURN);
    if (_board[tr][tc] && _board[tr][tc]->getColor()==movingColor) throw MoveException(MOVE_INVALID_TARGET_OCCUPIED);
    if (_board[tr][tc] && _board[tr][tc]->getType()=="King") throw MoveException(MOVE_INVALID_ILLEGAL_PIECE_MOVE);
    if (!isPseudoLegal(*piece,tr,tc,_board[tr][tc]!=nullptr)) throw MoveException(MOVE_INVALID_ILLEGAL_PIECE_MOVE);

    Square captured=std::move(_board[tr][tc]);
    _board[tr][tc]=std::move(_board[fr][fc]);
    _board[tr][tc]->setPosition(to);
    if (isCheck(movingColor)) {
        _board[fr][fc]=std::move(_board[tr][tc]);
        _board[fr][fc]->setPosition(from);
        _board[tr][tc]=std::move(captured);
        throw MoveException(MOVE_INVALID_CAUSES_SELF_CHECK);
    }
    _whiteTurn=!_whiteTurn;
    return isCheck(movingColor==WHITE?BLACK:WHITE) ? MOVE_VALID_CHECK : MOVE_VALID;
}

std::ostream& operator<<(std::ostream& os, const Board& board) {
    const std::string data=board.toString();
    for (int i=0;i<64;i+=8) os << data.substr(i,8) << '\n';
    return os;
}
