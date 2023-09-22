#include <iostream>

#include "Neptune/Board.hpp"

int main() {
  Board board;
  board.Reset();

  auto moves = board.GenerateLegalMoves(WHITE);

  for (auto move : moves) {
    std::cout << move.ToAlgebraicNotation() << std::endl;
  }

  return 0;
}
