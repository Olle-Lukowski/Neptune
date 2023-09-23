#include "Board.hpp"

#include <iostream>

// Assuming you have a function to generate a Bitboard with single bit set at a given index
Bitboard SingleBit(int index) {
  Bitboard bb;
  bb.SetBit(index);
  return bb;
}

// For white
Bitboard WHITE_QUEENSIDE_EMPTY_SQUARES_MASK = SingleBit(1) | SingleBit(2) | SingleBit(3);
Bitboard WHITE_KINGSIDE_EMPTY_SQUARES_MASK = SingleBit(5) | SingleBit(6);

Bitboard WHITE_QUEENSIDE_PASSING_KING_SQUARE = SingleBit(2) | SingleBit(3);
Bitboard WHITE_KINGSIDE_PASSING_KING_SQUARE = SingleBit(5) | SingleBit(6);

// For black
Bitboard BLACK_QUEENSIDE_EMPTY_SQUARES_MASK = SingleBit(57) | SingleBit(58) | SingleBit(59);
Bitboard BLACK_KINGSIDE_EMPTY_SQUARES_MASK = SingleBit(61) | SingleBit(62);

Bitboard BLACK_QUEENSIDE_PASSING_KING_SQUARE = SingleBit(58) | SingleBit(59);
Bitboard BLACK_KINGSIDE_PASSING_KING_SQUARE = SingleBit(61) | SingleBit(62);

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
      // tracking if king & rook moved for castling
      if (piece == KING) {
        kingMoved[color] = true;

        // castling
        switch (move.toSquare) {
          case WHITE_QUEENSIDE_CASTLE_TO_SQAURE:
            // move the rook
            pieces[color][ROOK].ClearBit(WHITE_QUEENSIDE_ROOK_FROM_SQUARE);
            pieces[color][ROOK].SetBit(WHITE_QUEENSIDE_ROOK_TO_SQUARE);
            break;
          case WHITE_KINGSIDE_CASTLE_TO_SQAURE:
            // move the rook
            pieces[color][ROOK].ClearBit(WHITE_KINGSIDE_ROOK_FROM_SQUARE);
            pieces[color][ROOK].SetBit(WHITE_KINGSIDE_ROOK_TO_SQUARE);
            break;
          case BLACK_QUEENSIDE_CASTLE_TO_SQAURE:
            // move the rook
            pieces[color][ROOK].ClearBit(BLACK_QUEENSIDE_ROOK_FROM_SQUARE);
            pieces[color][ROOK].SetBit(BLACK_QUEENSIDE_ROOK_TO_SQUARE);
            break;
          case BLACK_KINGSIDE_CASTLE_TO_SQAURE:
            // move the rook
            pieces[color][ROOK].ClearBit(BLACK_KINGSIDE_ROOK_FROM_SQUARE);
            pieces[color][ROOK].SetBit(BLACK_KINGSIDE_ROOK_TO_SQUARE);
            break;
        }
      }
      if (piece == ROOK) {
        if (move.fromSquare == 0 || move.fromSquare == 56) {
          rookMoved[color][0] = true;
        }
        if (move.fromSquare == 7 || move.fromSquare == 63) {
          rookMoved[color][1] = true;
        }
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
          // castling
          if (!kingMoved[color]) {
            // queenside 
            if (color == WHITE) {
              // Queenside
              if (!rookMoved[color][0] && (occupied & WHITE_QUEENSIDE_EMPTY_SQUARES_MASK).IsEmpty() && !IsSquareAttacked(WHITE_QUEENSIDE_PASSING_KING_SQUARE, !color)) {
                potentialMoves.SetBit(WHITE_QUEENSIDE_CASTLE_TO_SQAURE);
              }
              // Kingside
              if (!rookMoved[color][1] && (occupied & WHITE_KINGSIDE_EMPTY_SQUARES_MASK).IsEmpty() && !IsSquareAttacked(WHITE_KINGSIDE_PASSING_KING_SQUARE, !color)) {
                potentialMoves.SetBit(WHITE_KINGSIDE_CASTLE_TO_SQAURE);
              }
            } else {
              // Queenside
              if (!rookMoved[color][0] && (occupied & BLACK_QUEENSIDE_EMPTY_SQUARES_MASK).IsEmpty() && !IsSquareAttacked(BLACK_QUEENSIDE_PASSING_KING_SQUARE, !color)) {
                potentialMoves.SetBit(BLACK_QUEENSIDE_CASTLE_TO_SQAURE);
              }
              // Kingside
              if (!rookMoved[color][1] && (occupied & BLACK_KINGSIDE_EMPTY_SQUARES_MASK).IsEmpty() && !IsSquareAttacked(BLACK_KINGSIDE_PASSING_KING_SQUARE, !color)) {
                potentialMoves.SetBit(BLACK_KINGSIDE_CASTLE_TO_SQAURE);
              }
            }
          }
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
      potentialMoves &= slidingMoves;
    } else if (pieceType == BISHOP) {
      Bitboard slidingMoves = GenerateBishopMovesFromSquare(square);
      potentialMoves &= slidingMoves;
    } else if (pieceType == QUEEN) {
      Bitboard diagonalMoves = GenerateBishopMovesFromSquare(square);
      Bitboard straightMoves = GenerateRookMovesFromSquare(square);
      Bitboard slidingMoves = diagonalMoves | straightMoves;
      potentialMoves &= slidingMoves;
    }
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

bool Board::IsSquareAttacked(int square, int attackerColor) {
    // For each type of attacking piece, generate the moves that would attack the square.
    // Then intersect it with the positions of those pieces on the board. If non-empty, the square is attacked.
  
    // Check for pawn attacks
    Bitboard potentialPawnAttacks = (attackerColor == WHITE) ? pawnMoves[BLACK][square] : pawnMoves[WHITE][square];
    if ((potentialPawnAttacks & pieces[attackerColor][PAWN]).IsNotEmpty()) {
        return true;
    }

    // Check for knight attacks
    if ((knightMoves[square] & pieces[attackerColor][KNIGHT]).IsNotEmpty()) {
        return true;
    }

    // Check for bishop attacks
    Bitboard bishopAttackers = GenerateBishopMovesFromSquare(square) & pieces[attackerColor][BISHOP];
    if (bishopAttackers.IsNotEmpty()) {
        return true;
    }

    // Check for rook attacks
    Bitboard rookAttackers = GenerateRookMovesFromSquare(square) & pieces[attackerColor][ROOK];
    if (rookAttackers.IsNotEmpty()) {
        return true;
    }

    // Check for queen attacks
    Bitboard queenAttackers = (GenerateBishopMovesFromSquare(square) | GenerateRookMovesFromSquare(square)) & pieces[attackerColor][QUEEN];
    if (queenAttackers.IsNotEmpty()) {
        return true;
    }

    // Check for king attacks
    if ((kingMoves[square] & pieces[attackerColor][KING]).IsNotEmpty()) {
        return true;
    }
  
    return false;
}

bool Board::IsSquareAttacked(Bitboard targetSquares, int attackerColor) {
    while (!targetSquares.IsEmpty()) {
        int square = targetSquares.PopLeastSignificantBit();
        if (IsSquareAttacked(square, attackerColor)) {
            return true;
        }
    }
    return false;
}

