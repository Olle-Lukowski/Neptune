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

  void MakeMove(Move move, int color);
  
  std::vector<Move> GenerateLegalMoves(int color);

private:
  void InitMoves();

  bool IsMovePuttingKingInCheck(Move move, int color, int pieceType);
  Bitboard GenerateAllAttackedSquares(int color);

  Bitboard GenerateRookMovesFromSquare(int square);
  Bitboard GenerateBishopMovesFromSquare(int square);

  Bitboard MaskOffIllegalMoves(Bitboard potentialMoves, int color, int pieceType, int square);
  void AddMovesToList(std::vector<Move> &moveList, Bitboard legalMoves, int fromSquare, int color, int pieceType);

private:
  Bitboard pieces[2][6];
  Bitboard occupied;
  Bitboard occupiedColor[2];
  Bitboard pawnMoves[2][64];
  Bitboard pawnCaptureMoves[2][64];
  Bitboard knightMoves[64];
  Bitboard bishopMoves[64];
  Bitboard rookMoves[64];
  Bitboard queenMoves[64];
  Bitboard kingMoves[64];

};

#endif // NP_BOARD_HPP
