#ifndef NP_MOVE_HPP
#define NP_MOVE_HPP

#include <string>
#include <sstream>

class Move {
public:
  int fromSquare;
  int toSquare;
  bool isCastle;
  int promotionPiece = -1;

public:
  Move(int from, int to) : fromSquare(from), toSquare(to) {}

  std::string ToAlgebraicNotation() const {
    return SquareToAlgebraic(fromSquare) + SquareToAlgebraic(toSquare);
  }

private:
  static std::string SquareToAlgebraic(int square) {
    int x = square % 8;
    int y = square / 8;  // Flip the rank because 0,0 is the bottom-left in most chess libraries

    std::stringstream ss;
    ss << char('a' + x) << (y + 1);
    return ss.str();
  }
};


#endif // NP_MOVE_HPP
