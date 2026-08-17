#include "King.h"
#include <cstdlib>
King::King(char color, const std::string& position) : Piece(color, position) {}
bool King::canMove(const std::string& to) const { validatePosition(to); const int dx=std::abs(to[0]-_position[0]), dy=std::abs(to[1]-_position[1]); return to != _position && dx<=1 && dy<=1; }
void King::move(const std::string& to) { Piece::move(to); }
std::string King::getType() const { return "King"; }
