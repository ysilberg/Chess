#pragma once
#include <exception>

enum Status { MOVE_VALID=0, MOVE_VALID_CHECK=1, MOVE_INVALID_SOURCE_EMPTY=2,
    MOVE_INVALID_TARGET_OCCUPIED=3, MOVE_INVALID_CAUSES_SELF_CHECK=4,
    MOVE_INVALID_OUT_OF_BOUNDS=5, MOVE_INVALID_ILLEGAL_PIECE_MOVE=6,
    MOVE_INVALID_IDENTICAL_SQUARES=7, MOVE_INVALID_TURN=8, MOVE_VALID_CHECKMATE=9 };

class MoveException final : public std::exception {
public:
    explicit MoveException(Status code) noexcept : _errorCode(code) {}
    Status getErrorCode() const noexcept { return _errorCode; }
    const char* what() const noexcept override {
        static const char* messages[] = {"0","1","2","3","4","5","6","7","8","9"};
        return _errorCode >= MOVE_VALID && _errorCode <= MOVE_VALID_CHECKMATE
            ? messages[static_cast<int>(_errorCode)] : "Unknown move error.";
    }
private:
    Status _errorCode;
};
