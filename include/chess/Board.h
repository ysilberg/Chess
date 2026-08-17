#pragma once
#include "MoveException.h"
#include "Piece.h"
#include <array>
#include <iosfwd>
#include <memory>
#include <string>
#include <utility>

class Board {
public:
    explicit Board(const std::string& boardData);
    Board(const Board&) = delete;
    Board& operator=(const Board&) = delete;
    Board(Board&&) noexcept = default;
    Board& operator=(Board&&) noexcept = default;

    const Piece* getSymbol(const std::string& position) const;
    Piece* getSymbol(const std::string& position);
    std::string toString() const;
    bool whiteToMove() const noexcept { return _whiteTurn; }
    Status movePiece(const std::string& from, const std::string& to);
    bool isCheck(char color) const;
    friend std::ostream& operator<<(std::ostream& os, const Board& board);

private:
    using Square = std::unique_ptr<Piece>;
    std::array<std::array<Square, CHESS_SIZE>, CHESS_SIZE> _board{};
    bool _whiteTurn = true;

    static std::pair<int,int> indices(const std::string& position);
    bool isPathClear(int fromRow, int fromCol, int toRow, int toCol) const;
    bool isPseudoLegal(const Piece& piece, int toRow, int toCol, bool capture) const;
    bool isSquareAttacked(int row, int col, char byColor) const;
};
