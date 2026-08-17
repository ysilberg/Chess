#ifndef BISHOP_H
#define BISHOP_H

#include "Piece.h"
#include <cmath> // For std::abs
#include <string>

class Bishop : public Piece {
public:
    // Constructor
    Bishop(char color, const std::string& position);
    bool canMove(const std::string& newPosition) const override;
    void move(const std::string& newPosition) override;
    std::string getType() const override;
};

#endif // BISHOP_H
