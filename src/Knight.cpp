#include "chess/Knight.h"
#include <cstdlib>
Knight::Knight(char color, const std::string& position) : Piece(color, position) {}
bool Knight::canMove(const std::string& to) const { validatePosition(to); const int dx=std::abs(to[0]-_position[0]), dy=std::abs(to[1]-_position[1]); return (dx==1&&dy==2)||(dx==2&&dy==1); }
void Knight::move(const std::string& to) { Piece::move(to); }
std::string Knight::getType() const { return "Knight"; }
