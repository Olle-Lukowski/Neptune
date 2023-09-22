#include "Board.hpp"

Board::Board() {
  InitMoves();
}

void Board::Reset() {
  for (int color = WHITE; color <= BLACK; ++color) {
    for (int piece = PAWN; piece <= KING; ++piece) {
      pieces[color][piece].SetBoard(EMPTY_BOARD);
    }
  }

  pieces[WHITE][PAWN].SetBoard(WHITE_PAWN_STARTING_BOARD);
  pieces[WHITE][KNIGHT].SetBoard(WHITE_KNIGHT_STARTING_BOARD);
  pieces[WHITE][BISHOP].SetBoard(WHITE_BISHOP_STARTING_BOARD);
  pieces[WHITE][ROOK].SetBoard(WHITE_ROOK_STARTING_BOARD);
  pieces[WHITE][QUEEN].SetBoard(WHITE_QUEEN_STARTING_BOARD);
  pieces[WHITE][KING].SetBoard(WHITE_KING_STARTING_BOARD);

  pieces[BLACK][PAWN].SetBoard(BLACK_PAWN_STARTING_BOARD);
  pieces[BLACK][KNIGHT].SetBoard(BLACK_KNIGHT_STARTING_BOARD);
  pieces[BLACK][BISHOP].SetBoard(BLACK_BISHOP_STARTING_BOARD);
  pieces[BLACK][ROOK].SetBoard(BLACK_ROOK_STARTING_BOARD);
  pieces[BLACK][QUEEN].SetBoard(BLACK_QUEEN_STARTING_BOARD);
  pieces[BLACK][KING].SetBoard(BLACK_KING_STARTING_BOARD);
}

void Board::Log() {
  for (int color = WHITE; color <= BLACK; ++color) {
    for (int piece = PAWN; piece <= KING; ++piece) {
      pieces[color][piece].Log();
    }
  }
}

std::vector<Move> Board::GenerateLegalMoves(int color) {
  std::vector<Move> legalMoves;

  for (int pieceType = PAWN; pieceType <= KING; ++pieceType) {
    Bitboard currentPieces = pieces[color][pieceType];
    
    while (!currentPieces.IsEmpty()) {
      int square = currentPieces.PopLeastSignificantBit();
      
      Bitboard potentialMoves;
      switch (pieceType) {
        case PAWN:
          potentialMoves = pawnMoves[color][square];
          break;
        case KNIGHT:
          potentialMoves = knightMoves[square];
          break;
        case BISHOP:
          potentialMoves = bishopMoves[square];
          break;
        case ROOK:
          potentialMoves = rookMoves[square];
          break;
        case QUEEN:
          potentialMoves = queenMoves[square];
          break;
        case KING:
          potentialMoves = kingMoves[square];
          break;
      }

      // Mask off illegal moves based on board state
      Bitboard legalMoveBoard = MaskOffIllegalMoves(potentialMoves, color);

      // Convert this legal move board to a list of moves and append to legalMoves
      AddMovesToList(legalMoves, legalMoveBoard, square);
    }
  }

  return legalMoves;
}

void Board::InitMoves() {
  Bitboard bb;
  for (int square = 0; square < 64; ++square) {
    pawnMoves[WHITE][square] = bb.WhitePawnMoves(square);
    pawnMoves[BLACK][square] = bb.BlackPawnMoves(square);
    knightMoves[square] = bb.KnightMoves(square);
    bishopMoves[square] = bb.BishopMoves(square);
    rookMoves[square] = bb.RookMoves(square);
    kingMoves[square] = bb.KingMoves(square);
    queenMoves[square] = bishopMoves[square] | rookMoves[square];
  }
}

Bitboard Board::MaskOffIllegalMoves(Bitboard potentialMoves, int color) {
  // Mask off squares that contain pieces of the same color
  for (int piece = PAWN; piece <= KING; ++piece) {
    potentialMoves &= ~pieces[color][piece];
  }
  
  // Further logic for pawn captures, castling, etc.
  
  return potentialMoves;
}

void Board::AddMovesToList(std::vector<Move> &moveList, Bitboard legalMoves, int fromSquare) {

  while (!legalMoves.IsEmpty()) {
    int toSquare = legalMoves.PopLeastSignificantBit();
    Move move(fromSquare, toSquare);
    moveList.push_back(move);
  }
}
