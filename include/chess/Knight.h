#ifndef KNIGHT_H
#define KNIGHT_H

#include <stdexcept>

#include "Piece.h"

class Knight : public Piece {
public:
    Knight(char col, const std::string& pos);

    void move(const std::string& newPosition) override;

    bool canMove(const std::string& newPosition) const override;
    std::string getType() const override;

};

#endif // KNIGHT_H
