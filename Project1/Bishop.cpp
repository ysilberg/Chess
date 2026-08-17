#include "Bishop.h"
#include <cstdlib>
Bishop::Bishop(char color, const std::string& position) : Piece(color, position) {}
bool Bishop::canMove(const std::string& to) const { validatePosition(to); return to != _position && std::abs(to[0]-_position[0]) == std::abs(to[1]-_position[1]); }
void Bishop::move(const std::string& to) { Piece::move(to); }
std::string Bishop::getType() const { return "Bishop"; }
