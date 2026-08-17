#include "chess/Bishop.h"
#include "chess/Board.h"
#include "chess/King.h"
#include "chess/Knight.h"
#include "chess/Pawn.h"
#include "chess/Queen.h"
#include "chess/Rook.h"

#include <functional>
#include <initializer_list>
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>

namespace {
int failures = 0;

void check(bool condition, const char* name) {
    if (!condition) {
        ++failures;
        std::cerr << "FAIL: " << name << '\n';
    }
}

void expectMoveError(Status expected, const std::function<void()>& action, const char* name) {
    try {
        action();
        ++failures;
        std::cerr << "FAIL: " << name << " (no exception)\n";
    } catch (const MoveException& error) {
        check(error.getErrorCode() == expected, name);
    }
}

template<typename Exception>
void expectException(const std::function<void()>& action, const char* name) {
    try {
        action();
        ++failures;
        std::cerr << "FAIL: " << name << " (no exception)\n";
    } catch (const Exception&) {
        // Expected.
    } catch (...) {
        ++failures;
        std::cerr << "FAIL: " << name << " (wrong exception type)\n";
    }
}

std::string position(
    std::initializer_list<std::pair<const char*, char>> pieces,
    bool whiteToMove = true) {
    std::string data(64, '#');
    for (const auto& [square, piece] : pieces) {
        data[(7 - (square[1] - '1')) * 8 + (square[0] - 'a')] = piece;
    }
    data += whiteToMove ? '0' : '1';
    return data;
}

void testPieceGeometry() {
    Rook rook(WHITE, "d4");
    check(rook.canMove("d8") && rook.canMove("a4") && !rook.canMove("e5"), "rook geometry");

    Bishop bishop(WHITE, "d4");
    check(bishop.canMove("a1") && bishop.canMove("g7") && !bishop.canMove("d5"), "bishop geometry");

    Knight knight(WHITE, "d4");
    check(knight.canMove("f5") && knight.canMove("b3") && !knight.canMove("f6"), "knight geometry");

    Queen queen(WHITE, "d4");
    check(queen.canMove("h8") && queen.canMove("a4") && !queen.canMove("f5"), "queen geometry");

    King king(WHITE, "d4");
    check(king.canMove("e5") && !king.canMove("f4") && !king.canMove("d4"), "king geometry");

    Pawn whitePawn(WHITE, "e2");
    Pawn blackPawn(BLACK, "e7");
    check(whitePawn.canMove("e4") && whitePawn.canMove("d3") && blackPawn.canMove("e5"), "pawn geometry and direction");
    whitePawn.move("e3");
    check(!whitePawn.canMove("e5"), "pawn double move is only available initially");

    expectMoveError(MOVE_INVALID_OUT_OF_BOUNDS, [&] { rook.canMove("i4"); }, "piece boundary validation");
}

void testBoardParsingAndInput() {
    const std::string initial = "rnbqkbnrpppppppp################################PPPPPPPPRNBQKBNR0";
    Board board(initial);
    check(board.toString() == initial && board.whiteToMove(), "board parse and serialization round trip");

    expectException<std::invalid_argument>([] { Board board("short"); }, "reject short board data");
    expectException<std::invalid_argument>([] { Board board(std::string(64, '#') + "x"); }, "reject invalid turn marker");
    expectException<std::invalid_argument>([] { Board board(std::string(63, '#') + "z0"); }, "reject invalid piece symbol");

    expectMoveError(MOVE_INVALID_OUT_OF_BOUNDS, [&] { board.movePiece("e", "e4"); }, "reject short coordinate");
    expectMoveError(MOVE_INVALID_OUT_OF_BOUNDS, [&] { board.movePiece("e2", "i4"); }, "reject off-board coordinate");
    expectMoveError(MOVE_INVALID_IDENTICAL_SQUARES, [&] { board.movePiece("e2", "e2"); }, "reject identical squares");
    check(board.toString() == initial && board.whiteToMove(), "invalid input preserves board and turn");
}

void testTurnsAndCaptures() {
    Board board("rnbqkbnrpppppppp################################PPPPPPPPRNBQKBNR0");
    check(board.movePiece("e2", "e4") == MOVE_VALID && !board.whiteToMove(), "white move advances turn");
    const std::string afterWhite = board.toString();
    expectMoveError(MOVE_INVALID_TURN, [&] { board.movePiece("d2", "d4"); }, "turn enforcement");
    check(board.toString() == afterWhite && !board.whiteToMove(), "wrong-turn move preserves state");
    check(board.movePiece("d7", "d5") == MOVE_VALID && board.whiteToMove(), "black move advances turn");
    check(board.movePiece("e4", "d5") == MOVE_VALID, "pawn capture");
    check(board.getSymbol("d5") && board.getSymbol("d5")->getColor() == WHITE && !board.getSymbol("e4"), "capture updates board consistently");
}

void testObstructionAndOccupancy() {
    Board rookBoard(position({{"e1", 'K'}, {"e8", 'k'}, {"a1", 'R'}, {"a2", 'P'}}));
    expectMoveError(MOVE_INVALID_ILLEGAL_PIECE_MOVE, [&] { rookBoard.movePiece("a1", "a8"); }, "rook path obstruction");
    expectMoveError(MOVE_INVALID_TARGET_OCCUPIED, [&] { rookBoard.movePiece("a1", "a2"); }, "friendly occupancy");
    expectMoveError(MOVE_INVALID_SOURCE_EMPTY, [&] { rookBoard.movePiece("b1", "b2"); }, "empty source");

    Board bishopBoard(position({{"e1", 'K'}, {"e8", 'k'}, {"c1", 'B'}, {"d2", 'P'}}));
    expectMoveError(MOVE_INVALID_ILLEGAL_PIECE_MOVE, [&] { bishopBoard.movePiece("c1", "h6"); }, "bishop path obstruction");

    Board queenBoard(position({{"e1", 'K'}, {"e8", 'k'}, {"a1", 'Q'}, {"d4", 'P'}}));
    expectMoveError(MOVE_INVALID_ILLEGAL_PIECE_MOVE, [&] { queenBoard.movePiece("a1", "h8"); }, "queen path obstruction");

    Board knightBoard(position({{"e1", 'K'}, {"e8", 'k'}, {"b1", 'N'}, {"b2", 'P'}, {"c2", 'P'}}));
    check(knightBoard.movePiece("b1", "c3") == MOVE_VALID, "knight jumps over occupied squares");
}

void testPawnRules() {
    Board blockedForward(position({{"e1", 'K'}, {"e8", 'k'}, {"e2", 'P'}, {"e3", 'p'}}));
    expectMoveError(MOVE_INVALID_ILLEGAL_PIECE_MOVE, [&] { blockedForward.movePiece("e2", "e3"); }, "pawn cannot capture forward");
    expectMoveError(MOVE_INVALID_ILLEGAL_PIECE_MOVE, [&] { blockedForward.movePiece("e2", "e4"); }, "blocked pawn cannot double step");

    Board emptyDiagonal(position({{"e1", 'K'}, {"e8", 'k'}, {"e2", 'P'}}));
    expectMoveError(MOVE_INVALID_ILLEGAL_PIECE_MOVE, [&] { emptyDiagonal.movePiece("e2", "d3"); }, "pawn cannot move diagonally without capture");

    Board blackDouble(position({{"e1", 'K'}, {"e8", 'k'}, {"d7", 'p'}}, false));
    check(blackDouble.movePiece("d7", "d5") == MOVE_VALID, "black pawn double step");

    Board blackCapture(position({{"e1", 'K'}, {"e8", 'k'}, {"d4", 'p'}, {"c3", 'N'}}, false));
    check(blackCapture.movePiece("d4", "c3") == MOVE_VALID, "black pawn diagonal capture");
}

void testCheckAndKingSafety() {
    Board rookCheck(position({{"a1", 'K'}, {"e8", 'k'}, {"e1", 'R'}}));
    check(rookCheck.isCheck(BLACK), "rook check detection");

    Board knightCheck(position({{"a1", 'K'}, {"e8", 'k'}, {"f6", 'N'}}));
    check(knightCheck.isCheck(BLACK), "knight check detection");

    Board pawnCheck(position({{"a1", 'K'}, {"e8", 'k'}, {"d7", 'P'}}));
    check(pawnCheck.isCheck(BLACK), "pawn attack check detection");

    Board checkingMove(position({{"a1", 'K'}, {"e8", 'k'}, {"e1", 'R'}}));
    check(checkingMove.movePiece("e1", "e7") == MOVE_VALID_CHECK, "checking move status");

    Board resolveCheck(position({{"e1", 'K'}, {"a8", 'k'}, {"e8", 'r'}}));
    check(resolveCheck.isCheck(WHITE), "initial check state");
    check(resolveCheck.movePiece("e1", "d1") == MOVE_VALID && !resolveCheck.isCheck(WHITE), "king can resolve check");

    Board adjacentKing(position({{"e1", 'K'}, {"e3", 'k'}}));
    expectMoveError(MOVE_INVALID_CAUSES_SELF_CHECK, [&] { adjacentKing.movePiece("e1", "e2"); }, "king cannot move adjacent to enemy king");

    Board noKingCapture(position({{"a1", 'K'}, {"e8", 'k'}, {"e7", 'R'}}));
    expectMoveError(MOVE_INVALID_ILLEGAL_PIECE_MOVE, [&] { noKingCapture.movePiece("e7", "e8"); }, "king is never captured");
}

void testSelfCheckRollback() {
    Board pinned(position({{"e1", 'K'}, {"a8", 'k'}, {"e2", 'R'}, {"e8", 'r'}}));
    const std::string beforeMove = pinned.toString();
    expectMoveError(MOVE_INVALID_CAUSES_SELF_CHECK, [&] { pinned.movePiece("e2", "d2"); }, "self-check prevention");
    check(pinned.toString() == beforeMove && pinned.whiteToMove(), "non-capture rollback preserves board and turn");

    Board captureRollback(position({{"e1", 'K'}, {"a8", 'k'}, {"e2", 'R'}, {"e8", 'r'}, {"a2", 'n'}}));
    const std::string beforeCapture = captureRollback.toString();
    expectMoveError(MOVE_INVALID_CAUSES_SELF_CHECK, [&] { captureRollback.movePiece("e2", "a2"); }, "self-check capture rejected");
    check(captureRollback.toString() == beforeCapture, "capture rollback restores both pieces and turn");
    check(captureRollback.getSymbol("e2") && captureRollback.getSymbol("a2"), "capture rollback restores source and target occupancy");
}
}

int main() {
    testPieceGeometry();
    testBoardParsingAndInput();
    testTurnsAndCaptures();
    testObstructionAndOccupancy();
    testPawnRules();
    testCheckAndKingSafety();
    testSelfCheckRollback();

    if (failures != 0) {
        std::cerr << failures << " test(s) failed\n";
        return 1;
    }
    std::cout << "All chess core tests passed\n";
    return 0;
}
