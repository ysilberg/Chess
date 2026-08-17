# Engineering audit

Audit performed on the repository state at the start of `codex/chess-engine-audit`.

## Findings and resolutions

| Area | Finding | Resolution |
| --- | --- | --- |
| Ownership | `Board` owned raw pointers, had a destructor, and was implicitly copyable. `Manager::createBoard` copied it, risking double deletion and dangling pointers. | Replaced square ownership with `std::unique_ptr`; disabled copies and enabled moves. Removed the unused copying path. |
| Move transaction | A captured piece was deleted before self-check validation. Rollback moved the attacker back but could not restore the capture. | Moves now retain the captured `unique_ptr` until validation succeeds and restore the complete position on rejection. |
| Bounds/input | `Board::movePiece` indexed strings and board squares before validating them; the column upper-bound check also accepted index 8. | Coordinates and request length are validated before indexing. |
| Pawn rules | Pawns could move diagonally without capturing, capture forward, jump over a piece, and double-step from non-home squares depending on object history. | Board-aware pawn validation now separates quiet moves and captures, checks the intermediate square, and restricts double moves to the home rank. |
| Sliding paths | Row coordinates mixed one-based and zero-based indices, diagonal logic was inconsistent, and queen diagonal detection failed in one direction. | Replaced this with one coordinate conversion and a shared directional path walk. |
| Turn handling | Selecting an opponent piece returned the empty-source code and turn state lived in both `Manager` and `Board`. | `Board` is the single owner of turn state and returns the dedicated wrong-turn status. |
| Check/self-check | Check detection mixed coordinate systems and swallowed all exceptions. Move rollback was incomplete. | Attack evaluation now uses shared pseudo-legal geometry and obstruction checks; self-check rollback is transactional. |
| King behavior | A king could make a zero-length move, and the engine modeled a missing king as checkmate after allowing kings to be captured. | Identical moves are rejected centrally, attacked-square validation prevents unsafe king moves, and king capture is illegal. Checkmate remains explicitly unsupported. |
| IPC lifecycle | `Pipe` was implicitly copyable, did not initialize its handle, could double-close, and reported success after waiting without reopening the pipe. | Made the wrapper non-copyable and RAII-managed, made close idempotent, retried `CreateFile`, and terminated read buffers safely. |
| Controller | Legal moves were communicated by deliberately throwing exceptions; malformed messages could be sliced and indexed. | `Board::movePiece` returns successful statuses; exceptions are reserved for rejected moves; the controller validates four-character requests. |
| Build/test | README paths were wrong, GCC was claimed despite unconditional Win32 headers, and there was no test suite. | Added a portable C++17 CMake core/test target and retained the Windows Visual Studio application target. |

## Verified rule status

- Implemented and tested: normal movement for every piece, captures, friendly collision rejection, sliding obstruction, turns, coordinate validation, pawn home-rank double moves, check detection, moves giving check, king safety, and self-check prevention.
- Not implemented: checkmate, stalemate, castling, en passant, promotion, repetition, fifty-move rule, insufficient-material draws, move history, undo, clocks, and AI.

## Structure decision

The legacy `Project1/` directory was replaced after the initial audit with responsibility-based directories: `include/chess/` for the public core API, `src/` for portable implementation, `app/` for the Windows controller and IPC adapter, `visualstudio/` for IDE metadata, `gui/` for the supplied executable, and `docs/` and `tests/` for supporting material. CMake and Visual Studio references were updated together so both build paths consume the same source tree.

## Remaining manual validation

The chess core builds and passes its deterministic test executable with GCC using strict warnings plus AddressSanitizer and UndefinedBehaviorSanitizer. GitHub Actions now validates the CMake targets on Linux and Windows and separately compiles the checked-in Visual Studio solution. The named-pipe handshake and interactive bundled GUI still require a native Windows run because CI does not launch the GUI process.
