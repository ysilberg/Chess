#include "Pwn.h"
#include <cstdlib>
Pwn::Pwn(char color, const std::string& position) : Piece(color, position), _firstMove((color==WHITE&&position[1]=='2')||(color==BLACK&&position[1]=='7')) {}
bool Pwn::canMove(const std::string& to) const { validatePosition(to); const int direction=_color==WHITE?1:-1, dx=std::abs(to[0]-_position[0]), dy=to[1]-_position[1]; return (dx==0&&dy==direction)||(dx==0&&_firstMove&&dy==2*direction)||(dx==1&&dy==direction); }
void Pwn::move(const std::string& to) { Piece::move(to); _firstMove=false; }
std::string Pwn::getType() const { return "Pwn"; }
