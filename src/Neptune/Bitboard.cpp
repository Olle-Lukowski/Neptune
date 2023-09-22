#include "Bitboard.hpp"

#include <iostream>

void Bitboard::Log() {
  std::cout << "---------------------------------" << std::endl;
  for (int row = 7; row >= 0; --row) {
    for (int col = 0; col < 8; ++col) {
      int square = row * 8 + col;
      uint64_t mask = 1ULL << square;

      if (board & mask) {
        std::cout << "| 1 ";
      } else {
        std::cout << "| 0 ";
      }
    }

    std::cout << "|" << std::endl;
    std::cout << "---------------------------------" << std::endl;
  }
  std::cout << std::endl;
}
