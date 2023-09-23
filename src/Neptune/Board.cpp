#include "Board.hpp"

#include <iostream>

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

  occupied = pieces[WHITE][PAWN] | pieces[WHITE][KNIGHT] | pieces[WHITE][BISHOP] | pieces[WHITE][ROOK] | pieces[WHITE][QUEEN] | pieces[WHITE][KING] | pieces[BLACK][PAWN] | pieces[BLACK][KNIGHT] | pieces[BLACK][BISHOP] | pieces[BLACK][ROOK] | pieces[BLACK][QUEEN] | pieces[BLACK][KING];

  for (int piece = PAWN; piece <= KING; ++piece) {
    occupiedColor[WHITE] |= pieces[WHITE][piece];
    occupiedColor[BLACK] |= pieces[BLACK][piece];
  }
}

void Board::Log() {
  occupied.Log();
}

void Board::MakeMove(Move move, int color) {
  canEnPassant = false;

  for (int piece = PAWN; piece <= KING; ++piece) {
    bool actualType = false;
    // Remove the piece from its starting square
    if (pieces[color][piece].IsSet(move.fromSquare)) {
      pieces[color][piece].ClearBit(move.fromSquare);
      actualType = true;
    }
    // If there's an opposing piece at the destination square, remove it
    pieces[!color][piece].ClearBit(move.toSquare);
    if (actualType) {
      // Place the piece at its destination square
      pieces[color][piece].SetBit(move.toSquare);
      // en passant
      if (piece == PAWN && abs(move.fromSquare - move.toSquare) == 16) {
        canEnPassant = true;
      }
      // promotion
      if (move.promotionPiece != -1) {
        pieces[color][PAWN].ClearBit(move.toSquare);
        pieces[color][move.promotionPiece].SetBit(move.toSquare);
      }
    }
  }

  // Update the occupied squares
  occupied.SetBoard(EMPTY_BOARD);
  for (int c = WHITE; c <= BLACK; ++c) {
    for (int piece = PAWN; piece <= KING; ++piece) {
      occupied |= pieces[c][piece];
    }
  }

  for (int piece = PAWN; piece <= KING; ++piece) {
    occupiedColor[WHITE] |= pieces[WHITE][piece];
    occupiedColor[BLACK] |= pieces[BLACK][piece];
  }

  lastMove = move;
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
      Bitboard legalMoveBoard = MaskOffIllegalMoves(potentialMoves, color, pieceType, square);

      // Convert this legal move board to a list of moves and append to legalMoves
      AddMovesToList(legalMoves, legalMoveBoard, square, color, pieceType);

      // Inside GenerateLegalMoves function
      if (pieceType == PAWN) {
        Bitboard captureMoves = pawnCaptureMoves[color][square];

        Bitboard attackableSquares = occupiedColor[!color];

        if (canEnPassant) {
          if (color == WHITE) {
            attackableSquares.SetBit(lastMove.toSquare + 8);
          } else {
            attackableSquares.SetBit(lastMove.toSquare - 8);
          }
        }

        captureMoves &= attackableSquares; // Keep only squares occupied by the enemy
        // Convert this legal move board to a list of moves and append to legalMoves
        AddMovesToList(legalMoves, captureMoves, square, color, pieceType);
      }
    }
  }

  return legalMoves;
}

void Board::InitMoves() {
  Bitboard bb;
  for (int square = 0; square < 64; ++square) {
    pawnMoves[WHITE][square] = bb.WhitePawnMoves(square);
    pawnMoves[BLACK][square] = bb.BlackPawnMoves(square);
    pawnCaptureMoves[WHITE][square] = bb.WhitePawnCaptureMoves(square);
    pawnCaptureMoves[BLACK][square] = bb.BlackPawnCaptureMoves(square);
    knightMoves[square] = bb.KnightMoves(square);
    bishopMoves[square] = bb.BishopMoves(square);
    rookMoves[square] = bb.RookMoves(square);
    kingMoves[square] = bb.KingMoves(square);
    queenMoves[square] = bishopMoves[square] | rookMoves[square];
  }
}

bool Board::IsMovePuttingKingInCheck(Move move, int color, int pieceType) {
  // Temporarily make the move on the board
  Bitboard originalPiece = pieces[color][pieceType];
  pieces[color][pieceType].ClearBit(move.fromSquare);
  pieces[color][pieceType].SetBit(move.toSquare);

  // Update the occupied squares
  Bitboard originalOccupied = occupied;
  occupied.ClearBit(move.fromSquare);
  occupied.SetBit(move.toSquare);

  // Generate all squares attacked by the opponent
  Bitboard attackedSquares = GenerateAllAttackedSquares(!color);

  // Check if the king is in an attacked square
  bool isCheck = attackedSquares.IsSet(pieces[color][KING].GetLeastSignificantBit());

  // Unmake the move
  pieces[color][pieceType] = originalPiece;
  occupied = originalOccupied;

  return isCheck;
}

// Function to generate all attacked squares by a given color
Bitboard Board::GenerateAllAttackedSquares(int color) {
    Bitboard attackedSquares;
  
    for (int pieceType = PAWN; pieceType <= KING; ++pieceType) {
        Bitboard currentPieces = pieces[color][pieceType];
        
        while (!currentPieces.IsEmpty()) {
            int square = currentPieces.PopLeastSignificantBit();
            
            Bitboard potentialAttacks;
            switch (pieceType) {
                case PAWN:
                    potentialAttacks = pawnCaptureMoves[color][square];
                    break;
                case KNIGHT:
                    potentialAttacks = knightMoves[square];
                    break;
                case BISHOP:
                    potentialAttacks = GenerateBishopMovesFromSquare(square);
                    break;
                case ROOK:
                    potentialAttacks = GenerateRookMovesFromSquare(square);
                    break;
                case QUEEN:
                    potentialAttacks = GenerateBishopMovesFromSquare(square) | GenerateRookMovesFromSquare(square);
                    break;
                case KING:
                    potentialAttacks = kingMoves[square];
                    break;
            }

            // Add these potential attack squares to the overall attacked squares
            attackedSquares |= potentialAttacks;
        }
    }
  
    return attackedSquares;
}


Bitboard Board::GenerateRookMovesFromSquare(int square) {
  Bitboard attacks;

  Bitboard fileMask;
  Bitboard rankMask;
  fileMask.SetBoard(0x0101010101010101 << (square & 7));
  rankMask.SetBoard(0xFF << (8 * (square >> 3)));

  for (int to = square + 8; to <= 63; to += 8) {
        if (occupied.IsSet(to)) break;
        attacks.SetBit(to);
    }

    // South (down the file)
    for (int to = square - 8; to >= 0; to -= 8) {
        if (occupied.IsSet(to)) break;
        attacks.SetBit(to);
    }

    // East (along the rank to the right)
    for (int to = square + 1; to % 8 != 0; ++to) {
        if (occupied.IsSet(to)) break;
        attacks.SetBit(to);
    }

    // West (along the rank to the left)
    for (int to = square - 1; to % 8 != 7 && to >= 0; --to) {
        if (occupied.IsSet(to)) break;
        attacks.SetBit(to);
    }

    return attacks;
}

Bitboard Board::GenerateBishopMovesFromSquare(int square) {
    Bitboard moves;
    Bitboard blockers;
    int toSquare;

    // Directions: NW, NE, SW, SE
    int dx[] = {-1,  1, -1,  1};
    int dy[] = { 1,  1, -1, -1};

    for (int direction = 0; direction < 4; ++direction) {
        int x = square % 8;
        int y = square / 8;

        while (true) {
            x += dx[direction];
            y += dy[direction];
            toSquare = y * 8 + x;

            if (x < 0 || x >= 8 || y < 0 || y >= 8) {
                // We're outside the board, break out
                break;
            }

            // Add this mo can't jump over it, break outve
            Bitboard bb;
            bb.SetBit(toSquare);
            moves |= bb;

            // If this square is occupied, we can't jump over it, break out
            blockers = occupied & bb;
            if (!blockers.IsEmpty()) {
                break;
            }
        }
    }

    return moves;
}


Bitboard Board::MaskOffIllegalMoves(Bitboard potentialMoves, int color, int pieceType, int square) {
  // Mask off squares that contain pieces of the same color
  for (int piece = PAWN; piece <= KING; ++piece) {
    potentialMoves &= ~pieces[color][piece];
  }

  if (pieceType == ROOK || pieceType == BISHOP || pieceType == QUEEN) {
    Bitboard slidingMoves;
    if (pieceType == ROOK) {
      slidingMoves = GenerateRookMovesFromSquare(square);
    } else if (pieceType == BISHOP) {
      Bitboard slidingMoves = GenerateBishopMovesFromSquare(square);
      potentialMoves &= slidingMoves;
    } else if (pieceType == QUEEN) {
      Bitboard diagonalMoves = GenerateBishopMovesFromSquare(square);
      Bitboard straightMoves = GenerateRookMovesFromSquare(square);
      Bitboard slidingMoves = diagonalMoves | straightMoves;
      potentialMoves &= slidingMoves;
    }

    // Combine with the masked off potential moves
    potentialMoves &= slidingMoves;
  }
  
  // Further logic for pawn captures, castling, etc.
  
  return potentialMoves;
}

void Board::AddMovesToList(std::vector<Move> &moveList, Bitboard legalMoves, int fromSquare, int color, int pieceType) {

  while (!legalMoves.IsEmpty()) {
    int toSquare = legalMoves.PopLeastSignificantBit();
    Move move(fromSquare, toSquare);
    if (pieceType == PAWN && (move.toSquare >= 56 || move.toSquare <= 7)) {
      for (int piece = KNIGHT; piece < KING; ++piece) {
        Move promotionMove(fromSquare, toSquare);
        promotionMove.promotionPiece = piece;
        if (IsMovePuttingKingInCheck(promotionMove, color, pieceType)) {
          continue;
        }
        moveList.push_back(promotionMove);
      } 
    }
    if (IsMovePuttingKingInCheck(move, color, pieceType)) {
      continue;
    }
    moveList.push_back(move);
  }
}
