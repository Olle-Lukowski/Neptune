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
#define EMPTY -1

struct ColoredPiece {
  int pieceType;
  int pieceColor;
};

class Board {
public:
  Board();

  void Reset();
  void Log();

  void MakeMove(Move move, int color);
  
  std::vector<Move> GenerateLegalMoves(int color);

  void InitMoves();

  int EvaluateMaterial();
  int EvaluateBoard();

private:

  bool IsMovePuttingKingInCheck(Move move, int color, int pieceType);
  Bitboard GenerateAllAttackedSquares(int color);

  Bitboard GenerateRookMovesFromSquare(int square);
  Bitboard GenerateBishopMovesFromSquare(int square);

  Bitboard MaskOffIllegalMoves(Bitboard potentialMoves, int color, int pieceType, int square);
  void AddMovesToList(std::vector<Move> &moveList, Bitboard legalMoves, int fromSquare, int color, int pieceType);

  bool IsSquareAttacked(int square, int attackerColor);
  bool IsSquareAttacked(Bitboard targetSquares, int attackerColor);

  ColoredPiece GetPieceAt(int square);

private:
  Bitboard pieces[2][6];
  Bitboard occupied;
  Bitboard occupiedColor[2];

  bool canEnPassant;
  Move lastMove = {0, 0};

  bool kingMoved[2] = {false, false};
  bool rookMoved[2][2] = {{false, false}, {false, false}};
};

int MiniMax(Board board, int depth, bool isMaximizing);

#define WHITE_QUEENSIDE_CASTLE_TO_SQAURE 2
#define WHITE_KINGSIDE_CASTLE_TO_SQAURE 6
#define BLACK_QUEENSIDE_CASTLE_TO_SQAURE 58
#define BLACK_KINGSIDE_CASTLE_TO_SQAURE 62
// where the rook moves on castle
#define WHITE_QUEENSIDE_ROOK_TO_SQUARE 3
#define WHITE_KINGSIDE_ROOK_TO_SQUARE 5
#define BLACK_QUEENSIDE_ROOK_TO_SQUARE 59
#define BLACK_KINGSIDE_ROOK_TO_SQUARE 61
// where the rook moves from on castle
#define WHITE_QUEENSIDE_ROOK_FROM_SQUARE 0
#define WHITE_KINGSIDE_ROOK_FROM_SQUARE 7
#define BLACK_QUEENSIDE_ROOK_FROM_SQUARE 56
#define BLACK_KINGSIDE_ROOK_FROM_SQUARE 63


#endif // NP_BOARD_HPP
