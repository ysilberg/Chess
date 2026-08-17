#include "chess/Bishop.h"
#include "chess/Board.h"
#include "chess/King.h"
#include "chess/Knight.h"
#include "chess/Pawn.h"
#include "chess/Queen.h"
#include "chess/Rook.h"
#include <functional>
#include <iostream>
#include <map>
#include <stdexcept>

namespace {
int failures=0;
void check(bool condition, const char* name) { if (!condition) { ++failures; std::cerr << "FAIL: " << name << '\n'; } }
void expectError(Status expected, const std::function<void()>& action, const char* name) {
    try { action(); ++failures; std::cerr << "FAIL: " << name << " (no exception)\n"; }
    catch (const MoveException& e) { check(e.getErrorCode()==expected,name); }
}
std::string position(std::initializer_list<std::pair<const char*,char>> pieces, bool white=true) {
    std::string data(64,'#');
    for (const auto& item:pieces) {
        const std::string square=item.first;
        data[(7-(square[1]-'1'))*8+(square[0]-'a')]=item.second;
    }
    data += white?'0':'1'; return data;
}
}

int main() {
    Rook rook(WHITE,"d4"); check(rook.canMove("d8")&&!rook.canMove("e5"),"rook geometry");
    Bishop bishop(WHITE,"d4"); check(bishop.canMove("a1")&&!bishop.canMove("d5"),"bishop geometry");
    Knight knight(WHITE,"d4"); check(knight.canMove("f5")&&!knight.canMove("f6"),"knight geometry");
    Queen queen(WHITE,"d4"); check(queen.canMove("h8")&&queen.canMove("a4")&&!queen.canMove("f5"),"queen geometry");
    King king(WHITE,"d4"); check(king.canMove("e5")&&!king.canMove("f4")&&!king.canMove("d4"),"king geometry");
    Pawn whitePawn(WHITE,"e2"), blackPawn(BLACK,"e7");
    check(whitePawn.canMove("e4")&&whitePawn.canMove("d3")&&blackPawn.canMove("e5"),"pawn geometry and direction");
    expectError(MOVE_INVALID_OUT_OF_BOUNDS,[&]{ rook.canMove("i4"); },"piece boundary validation");

    Board start("rnbqkbnrpppppppp################################PPPPPPPPRNBQKBNR0");
    check(start.movePiece("e2","e4")==MOVE_VALID,"initial pawn double move");
    expectError(MOVE_INVALID_TURN,[&]{ start.movePiece("d2","d4"); },"turn enforcement");
    check(start.movePiece("d7","d5")==MOVE_VALID,"black move");
    check(start.movePiece("e4","d5")==MOVE_VALID,"pawn capture");
    check(start.getSymbol("d5") && start.getSymbol("d5")->getColor()==WHITE && !start.getSymbol("e4"),"capture updates board");

    Board blocked(position({{"e1",'K'},{"e8",'k'},{"a1",'R'},{"a2",'P'}}));
    expectError(MOVE_INVALID_ILLEGAL_PIECE_MOVE,[&]{ blocked.movePiece("a1","a8"); },"sliding path obstruction");
    expectError(MOVE_INVALID_TARGET_OCCUPIED,[&]{ blocked.movePiece("a1","a2"); },"own-piece collision");
    expectError(MOVE_INVALID_OUT_OF_BOUNDS,[&]{ blocked.movePiece("a1","a9"); },"board boundary validation");
    expectError(MOVE_INVALID_SOURCE_EMPTY,[&]{ blocked.movePiece("b1","b2"); },"empty source");

    Board pawnRules(position({{"e1",'K'},{"e8",'k'},{"e2",'P'},{"e3",'p'}}));
    expectError(MOVE_INVALID_ILLEGAL_PIECE_MOVE,[&]{ pawnRules.movePiece("e2","e3"); },"pawn cannot capture forward");
    Board pawnDiagonal(position({{"e1",'K'},{"e8",'k'},{"e2",'P'}}));
    expectError(MOVE_INVALID_ILLEGAL_PIECE_MOVE,[&]{ pawnDiagonal.movePiece("e2","d3"); },"pawn cannot move diagonally without capture");

    Board checkBoard(position({{"a1",'K'},{"e8",'k'},{"e1",'R'}}));
    check(checkBoard.isCheck(BLACK),"check detection");
    Board discovered(position({{"e1",'K'},{"a8",'k'},{"e2",'R'},{"e8",'r'}}));
    const std::string before=discovered.toString();
    expectError(MOVE_INVALID_CAUSES_SELF_CHECK,[&]{ discovered.movePiece("e2","d2"); },"self-check prevention");
    check(discovered.toString()==before,"failed move preserves board and turn");

    Board checkingMove(position({{"a1",'K'},{"e8",'k'},{"e1",'R'}}));
    check(checkingMove.movePiece("e1","e7")==MOVE_VALID_CHECK,"checking move status");
    Board noKingCapture(position({{"a1",'K'},{"e8",'k'},{"e7",'R'}}));
    expectError(MOVE_INVALID_ILLEGAL_PIECE_MOVE,[&]{ noKingCapture.movePiece("e7","e8"); },"king is never captured");
    Board kingDanger(position({{"e1",'K'},{"a8",'k'},{"e8",'r'}}));
    expectError(MOVE_INVALID_CAUSES_SELF_CHECK,[&]{ kingDanger.movePiece("e1","e2"); },"king cannot enter attack");

    if (failures) { std::cerr << failures << " test(s) failed\n"; return 1; }
    std::cout << "All chess core tests passed\n"; return 0;
}
