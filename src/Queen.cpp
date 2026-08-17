#include "chess/Queen.h"
#include <cstdlib>
Queen::Queen(char color, const std::string& position) : Piece(color, position) {}
bool Queen::canMove(const std::string& to) const { validatePosition(to); const int dx=std::abs(to[0]-_position[0]), dy=std::abs(to[1]-_position[1]); return to != _position && (dx==0||dy==0||dx==dy); }
void Queen::move(const std::string& to) { Piece::move(to); }
std::string Queen::getType() const { return "Queen"; }
