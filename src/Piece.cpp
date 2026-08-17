#include "chess/Piece.h"
#include "chess/MoveException.h"
#include <ostream>
#include <stdexcept>

Piece::Piece(char color, const std::string& position) : _color(color) {
    if (color != WHITE && color != BLACK) throw std::invalid_argument("Piece color must be 'w' or 'b'.");
    setPosition(position);
}
const std::string& Piece::getPosition() const noexcept { return _position; }
char Piece::getColor() const noexcept { return _color; }
void Piece::validatePosition(const std::string& position) {
    if (position.size() != 2 || position[0] < 'a' || position[0] > 'h' || position[1] < '1' || position[1] > '8')
        throw MoveException(MOVE_INVALID_OUT_OF_BOUNDS);
}
void Piece::setPosition(const std::string& position) { validatePosition(position); _position = position; }
void Piece::move(const std::string& newPosition) {
    if (!canMove(newPosition)) throw MoveException(MOVE_INVALID_ILLEGAL_PIECE_MOVE);
    setPosition(newPosition);
}
std::ostream& operator<<(std::ostream& os, const Piece& piece) {
    return os << "Piece: " << piece.getType() << ", Color: "
              << (piece._color == WHITE ? "White" : "Black") << ", Position: " << piece._position;
}
