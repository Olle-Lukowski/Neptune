#include <iostream>
#include <string>
#include <algorithm>

#include "Neptune/Board.hpp"

int main() {
  Board board;
  board.Reset();
  int currentPlayer = WHITE;

  while (true) {
    auto moves = board.GenerateLegalMoves(currentPlayer);
    if (moves.empty()) {
      std::cout << (currentPlayer == WHITE ? "White" : "Black") << " has no legal moves. Game over." << std::endl;
      break;
    }

    std::cout << "Current board:" << std::endl;
    board.Log();

    std::cout << (currentPlayer == WHITE ? "WHITE" : "BLACK") << " moves available:" << std::endl;
    for (const auto& move : moves) {
      std::cout << move.ToAlgebraicNotation() << " ";
    }
    std::cout << std::endl;

    std::string inputMoveStr;
    std::cout << "Enter your move: ";
    std::cin >> inputMoveStr;

    // Convert the input string to a Move object
    Move inputMove = Move::FromAlgebraicNotation(inputMoveStr);

    // Verify that the move is legal
    if (std::find(moves.begin(), moves.end(), inputMove) != moves.end()) {
      board.MakeMove(inputMove, currentPlayer);
    } else {
      std::cout << "Illegal move. Try again." << std::endl;
      continue;
    }

    // Switch the current player
    currentPlayer = (currentPlayer == WHITE) ? BLACK : WHITE;
  }

  return 0;
}

