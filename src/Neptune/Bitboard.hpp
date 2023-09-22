#ifndef NP_BITBOARD_HPP
#define NP_BITBOARD_HPP

#include <cstdint>

#if defined(_MSC_VER)
#include <intrin.h>
#endif

inline int BitScanForwardPortable(uint64_t bb) {
  int index = 0;
  while (!(bb & 1)) {
    bb >>= 1;
    ++index;
  }
  return index;
}

class Bitboard {
public:
  Bitboard() : board(0) {}

  inline void SetBit(int square) {
    board |= (1ULL << square);
  }

  inline void ClearBit(int square) {
    board &= ~(1ULL << square);
  }

  inline void Clear() {
    board = 0;
  }

  inline bool IsSet(int square) const {
    return (board & (1ULL << square)) != 0;
  }

  inline uint64_t GetBoard() const {
    return board;
  }

  inline void SetBoard(uint64_t b) {
    board = b;
  }

  inline Bitboard Inverse() const {
    Bitboard bb;
    bb.SetBoard(~board);
    return bb;
  }

  inline int PopLeastSignificantBit() {
    int lsbIndex;

#if defined(__GNUC__) || defined(__clang__)
    lsbIndex = __builtin_ctzll(board);
#elif defined(_MSC_VER)
    unsigned long index;  // Note: MSVC uses 'unsigned long'
    _BitScanForward64(&index, board);
    lsbIndex = static_cast<int>(index);
#else
    lsbIndex = BitScanForwardPortable(board); // Your portable version
#endif

    board &= (board - 1);
    return lsbIndex;
  }

  inline bool IsEmpty() const {
    return board == 0;
  }

  inline Bitboard WhitePawnMoves(int square) {
    Clear();
    uint64_t position = 1ULL << square;
    uint64_t emptySquares = ~position;

    board = (position << 8) & emptySquares;
    if (square >= 8 && square <= 15) {
      board |= (position << 16) & emptySquares;
    }

    return *this;
  }

  inline Bitboard BlackPawnMoves(int square) {
    Clear();
    uint64_t position = 1ULL << square;
    uint64_t emptySquares = ~position;

    board = (position >> 8) & emptySquares;
    if (square >= 48 && square <= 55) {
      board |= (position >> 16) & emptySquares;
    }

    return *this;
  }

  inline Bitboard KnightMoves(int square) {
    Clear();
    int dx[] = {2, 1, -1, -2, -2, -1, 1, 2};
    int dy[] = {1, 2, 2, 1, -1, -2, -2, -1};

    int x = square % 8;
    int y = square / 8;

    for (int i = 0; i < 8; ++i) {
      int newX = x + dx[i];
      int newY = y + dy[i];

      if (newX >= 0 && newX < 8 && newY >= 0 && newY < 8) {
        int newSquare = newY * 8 + newX;
        SetBit(newSquare);
      }
    }

    return *this;
  }

  inline Bitboard BishopMoves(int square) {
    Clear();
    int dx[] = {1, 1, -1, -1};
    int dy[] = {1, -1, 1, -1};

    int x = square % 8;
    int y = square / 8;

    for (int dir = 0; dir < 4; ++dir) {
      for (int step = 1; step < 8; ++step) {
        int newX = x + dx[dir] * step;
        int newY = y + dy[dir] * step;

        if (newX >= 0 && newX < 8 && newY >= 0 && newY < 8) {
          int newSquare = newY * 8 + newX;
          SetBit(newSquare);
        } else {
          // Off the board
          break;
        }
      }
    }

    return *this;
  }

  inline Bitboard RookMoves(int square) {
    Clear();
    int dx[] = {1, 0, -1, 0};  // Left and right
    int dy[] = {0, 1, 0, -1};  // Up and down
    
    int x = square % 8;
    int y = square / 8;

    for (int dir = 0; dir < 4; ++dir) {
      for (int step = 1; step < 8; ++step) {
        int newX = x + dx[dir] * step;
        int newY = y + dy[dir] * step;

        if (newX >= 0 && newX < 8 && newY >= 0 && newY < 8) {
          int newSquare = newY * 8 + newX;
          SetBit(newSquare);
        } else {
          // Off the board
          break;
        }
      }
    }

    return *this;
  }

  inline Bitboard KingMoves(int square) {
    Clear();
    int dx[] = {1, 1, 1, 0, -1, -1, -1, 0};
    int dy[] = {1, 0, -1, -1, -1, 0, 1, 1};

    int x = square % 8;
    int y = square / 8;

    for (int dir = 0; dir < 8; ++dir) {
      int newX = x + dx[dir];
      int newY = y + dy[dir];

      if (newX >= 0 && newX < 8 && newY >= 0 && newY < 8) {
        int newSquare = newY * 8 + newX;
        SetBit(newSquare);
      }
    }

    return *this;
  }

  Bitboard operator|(const Bitboard &bb) {
    Bitboard result;
    result.SetBoard(board | bb.GetBoard());
    return result;
  }

  Bitboard operator&(const Bitboard &bb) {
    Bitboard result;
    result.SetBoard(board & bb.GetBoard());
    return result;
  }

  Bitboard &operator|=(const Bitboard &bb) {
    board |= bb.GetBoard();
    return *this;
  }

  Bitboard &operator&=(const Bitboard &bb) {
    board &= bb.GetBoard();
    return *this;
  }

  Bitboard operator~() {
    Bitboard result;
    result.SetBoard(~board);
    return result;
  }

  void Log();
  
private:
  uint64_t board;
};

#define EMPTY_BOARD 0ULL
#define WHITE_PAWN_STARTING_BOARD 0xFF00ULL
#define WHITE_KNIGHT_STARTING_BOARD 0x42ULL
#define WHITE_BISHOP_STARTING_BOARD 0x24ULL
#define WHITE_ROOK_STARTING_BOARD 0x81ULL
#define WHITE_QUEEN_STARTING_BOARD 0x8ULL
#define WHITE_KING_STARTING_BOARD 0x10ULL

#define BLACK_PAWN_STARTING_BOARD 0xFF000000000000ULL
#define BLACK_KNIGHT_STARTING_BOARD 0x4200000000000000ULL
#define BLACK_BISHOP_STARTING_BOARD 0x2400000000000000ULL
#define BLACK_ROOK_STARTING_BOARD 0x8100000000000000ULL
#define BLACK_QUEEN_STARTING_BOARD 0x800000000000000ULL
#define BLACK_KING_STARTING_BOARD 0x1000000000000000ULL

#endif // NP_BITBOARD_HPP
