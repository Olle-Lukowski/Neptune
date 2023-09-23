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

  static Move FromAlgebraicNotation(const std::string& moveStr) {
    int fromSquare = AlgebraicToSquare(moveStr.substr(0, 2));
    int toSquare = AlgebraicToSquare(moveStr.substr(2, 2));
    return Move(fromSquare, toSquare);
  }

  bool operator==(const Move& other) const {
    return fromSquare == other.fromSquare && toSquare == other.toSquare;
  }

private:
  static std::string SquareToAlgebraic(int square) {
    int x = square % 8;
    int y = square / 8;  // Flip the rank because 0,0 is the bottom-left in most chess libraries

    std::stringstream ss;
    ss << char('a' + x) << (y + 1);
    return ss.str();
  }

  static int AlgebraicToSquare(const std::string& str) {
    int x = str[0] - 'a';
    int y = str[1] - '1';
    return y * 8 + x;
  }
};


#endif // NP_MOVE_HPP
