#pragma once
#include "Piece.h"

class Pawn : public Piece {
public:
    Pawn(char col, const std::string& pos);

    void move(const std::string& newPosition) override;

    bool canMove(const std::string& newPosition) const override;
    std::string getType() const override;

private:
    bool _firstMove;
};
