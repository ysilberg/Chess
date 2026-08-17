# Chess Project Magshimim

A two-player C++ chess engine that validates moves and communicates with a supplied Windows GUI over a named pipe. The project was collaboratively developed by **Dorian Salomon and Yan Silberg** as part of the Magshimim program.

This repository appears on GitHub as a fork because the original collaborative repository was hosted under Yan's account. It is not a third-party project: this repository preserves work created jointly by Dorian and Yan.

## Current feature set

The engine currently supports:

- legal movement geometry for kings, queens, rooks, bishops, knights, and pawns;
- turn enforcement, captures, friendly-piece collision checks, and obstruction checks for sliding pieces;
- pawn single moves, initial two-square moves, and diagonal captures;
- check detection and rejection of moves that leave the moving side's king in check;
- deterministic board serialization compatible with the GUI's 65-character protocol;
- protocol status codes for accepted moves, check, and invalid-move categories;
- Windows named-pipe communication with the included GUI executable.

The engine detects **check**, but it does not yet determine checkmate or stalemate. Castling, en passant, promotion, draw rules, move history, undo, clocks, and AI are also not implemented.

## Architecture

The code is divided into a platform-independent chess core and a thin Windows integration layer.

| Component | Responsibility |
| --- | --- |
| `Board` | Owns pieces, parses and serializes positions, validates board-dependent rules, applies moves transactionally, enforces turns, and detects check. |
| `Piece` | Abstract base class containing color and position state plus the movement interface. |
| Piece subclasses | Implement movement geometry for each piece type. Board-dependent concerns such as obstruction and occupancy stay in `Board`. |
| `Manager` | Translates four-character GUI requests into board operations and sends protocol status codes back to the GUI. |
| `MoveException` | Carries the numeric status code required by the GUI protocol. |
| `Pipe` | Owns the Windows named-pipe handle and performs synchronous IPC with `chessGraphics.exe`. |

`Board` uses `std::unique_ptr` for exclusive piece ownership. Copying is disabled, which prevents double deletion; moving is supported. A rejected self-check move restores both the moved piece and any captured piece before reporting the error.

### Board format and moves

Board state is represented by 64 characters ordered from rank 8 to rank 1, followed by a turn character (`0` for White, `1` for Black). Uppercase pieces are White, lowercase pieces are Black, and `#` is an empty square.

The GUI sends coordinate moves as four characters: source square followed by destination square, for example `e2e4`. Replies use the original assignment protocol:

| Code | Meaning |
| --- | --- |
| `0` | Legal move |
| `1` | Legal move that gives check |
| `2` | Source square is empty |
| `3` | Destination contains a friendly piece |
| `4` | Move leaves the moving side in check |
| `5` | Invalid coordinate or request length |
| `6` | Illegal movement or blocked path |
| `7` | Source and destination are identical |
| `8` | Wrong side attempted to move |

Code `9` is reserved by the original protocol, but the current engine does not produce it because checkmate detection is not implemented.

## Repository layout

```text
Chess-Project-Magshimim/
├── app/                    # Windows entry point, controller, and named-pipe adapter
├── include/chess/          # public chess-core headers
├── src/                    # platform-independent chess-core implementation
├── tests/                  # deterministic core tests
├── visualstudio/           # Visual Studio solution and project files
├── gui/                    # supplied prebuilt Windows GUI
├── docs/                   # engineering audit and original UML image
├── CMakeLists.txt          # portable core, tests, and Windows app build
├── LICENSE.md
└── README.md
```

The chess rules are isolated from the Windows integration. Consumers include headers from `include/chess/`; `src/` has no dependency on Win32 or the GUI. The `app/` layer depends on both the core and Windows APIs, while `visualstudio/` and the root CMake file provide two build entry points over the same sources.

## Build and test the chess core

The core and test suite require a C++17 compiler and CMake 3.16 or newer. They are platform-independent.

```powershell
git clone https://github.com/NRG-Wardog/Chess-Project-Magshimim.git
cd Chess-Project-Magshimim
cmake -S . -B build
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
```

On a single-configuration generator such as Ninja or Unix Makefiles, omit `-C Release` from the `ctest` command.

## Build and run with the Windows GUI

Requirements:

- Windows 10 or later;
- Visual Studio 2022 with the **Desktop development with C++** workload;
- a Windows 10/11 SDK.

1. Open `visualstudio/Chess.sln` in Visual Studio.
2. Select `Debug` or `Release` and an appropriate platform (`x64` is recommended).
3. Build the solution.
4. Start `gui/chessGraphics.exe`, which hosts `\\.\pipe\chessPipe`.
5. Run the built `Chess` console application. It connects to the GUI and processes moves until the GUI sends `quit`.

The portable CMake build creates `chess_app` only on Windows. The GUI executable is prebuilt and is not compiled from source in this repository.

## Error handling

Expected move failures use `MoveException` so the controller can return the exact GUI status code. Malformed board data and impossible engine states use standard exceptions. Pipe operations report Win32 failures and return failure values; the startup path allows a connection retry.

## Tests

`tests/test_chess.cpp` covers every piece's movement geometry, bounds checking, turn order, captures, friendly occupancy, sliding-piece obstruction, pawn occupancy rules, check detection, checking-move status, king safety, self-check prevention, and transactional rollback after a rejected move.

## Known limitations

- Checkmate and stalemate are not detected, so the engine does not terminate a completed game automatically.
- Castling, en passant, and pawn promotion are unsupported.
- Threefold repetition, the fifty-move rule, and insufficient-material draws are unsupported.
- The GUI/IPC path is Windows-only and synchronous.
- The bundled GUI has no source code here and must be validated manually on Windows.
- The board interchange format does not validate whether an imported position is a reachable or otherwise legal chess position.

## Future improvements

1. Add legal-move generation, then implement checkmate and stalemate from that shared primitive.
2. Add explicit move history to support castling, en passant, promotion, undo, and draw rules.
3. Replace protocol exceptions with a typed move result internally while keeping the numeric adapter at the IPC boundary.
4. Wrap the Win32 pipe in a smaller transport interface so `Manager` can be integration-tested with a fake connection.
5. Add Windows CI for the Visual Studio target and portable CI for the chess-core tests.

## License

See [LICENSE.md](LICENSE.md).
