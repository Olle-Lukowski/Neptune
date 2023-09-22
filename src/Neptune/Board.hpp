#ifndef NP_BOARD_HPP
#define NP_BOARD_HPP

#include <vector>

#include "Bitboard.hpp"
#include "Move.hpp"

#define WHITE 0
#define BLACK 1
#define PAWN 0
#define KNIGHT 1
#define BISHOP 2
#define ROOK 3
#define QUEEN 4
#define KING 5

class Board {
public:
  Board();

  void Reset();
  void Log();
  
  std::vector<Move> GenerateLegalMoves(int color);

private:
  void InitMoves();

  Bitboard MaskOffIllegalMoves(Bitboard potentialMoves, int color);
  void AddMovesToList(std::vector<Move> &moveList, Bitboard legalMoves, int fromSquare);

private:
  Bitboard pieces[2][6];
  Bitboard pawnMoves[2][64];
  Bitboard knightMoves[64];
  Bitboard bishopMoves[64];
  Bitboard rookMoves[64];
  Bitboard queenMoves[64];
  Bitboard kingMoves[64];

};

#endif // NP_BOARD_HPP
