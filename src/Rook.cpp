#include "chess/Rook.h"
Rook::Rook(char color, const std::string& position) : Piece(color, position) {}
bool Rook::canMove(const std::string& to) const { validatePosition(to); return to != _position && (to[0] == _position[0] || to[1] == _position[1]); }
void Rook::move(const std::string& to) { Piece::move(to); }
std::string Rook::getType() const { return "Rook"; }
