#pragma once
#include <iosfwd>
#include <string>

constexpr int CHESS_SIZE = 8;
constexpr int CHESS_BOARD_SIZE = 64;
constexpr char WHITE = 'w';
constexpr char BLACK = 'b';

class Piece {
public:
    Piece(char color, const std::string& position);
    virtual ~Piece() = default;
    const std::string& getPosition() const noexcept;
    char getColor() const noexcept;
    void setPosition(const std::string& position);
    virtual void move(const std::string& newPosition);
    virtual bool canMove(const std::string& newPosition) const = 0;
    virtual std::string getType() const = 0;
    friend std::ostream& operator<<(std::ostream& os, const Piece& piece);
protected:
    static void validatePosition(const std::string& position);
    std::string _position;
    char _color;
};
