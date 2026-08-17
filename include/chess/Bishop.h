#pragma once
#include "Piece.h"

class Bishop : public Piece {
public:
    Bishop(char color, const std::string& position);
    bool canMove(const std::string& newPosition) const override;
    void move(const std::string& newPosition) override;
    std::string getType() const override;
};
