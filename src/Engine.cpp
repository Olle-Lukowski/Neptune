#include <iostream>
#include <string>
#include <algorithm>

#include "Neptune/Board.hpp"

int main() {
  Board board;
  board.Reset();
  board.InitMoves();
  int currentPlayer = WHITE;

  while (true) {
    // Generate legal moves for the current player
    auto moves = board.GenerateLegalMoves(currentPlayer);
    
    if (moves.empty()) {
      std::cout << "Game over. " << (currentPlayer == WHITE ? "White" : "Black") << " has no legal moves." << std::endl;
      break;
    }

    std::cout << "Current board:" << std::endl;
    board.Log();
    std::cout << "Eval: " << board.EvaluateBoard() << std::endl;

    // If it's White's turn, let the human decide
    if (currentPlayer == WHITE) {
      std::cout << "WHITE moves available:" << std::endl;
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

    } else { // If it's Black's turn, let the engine decide
      int bestMoveValue = 9999;
      Move bestMove(-1, -1);

      for (Move move : moves) {
        Board tempBoard = board;
        tempBoard.MakeMove(move, currentPlayer);
        int moveValue = MiniMax(tempBoard, 4, true);  // Depth 3, and White's turn to move next
        std::cout << moveValue << std::endl;

        if (moveValue < bestMoveValue) {
          bestMoveValue = moveValue;
          bestMove = move;
        }
      }

      // Make the best move for Black
      std::cout << "Engine chooses move: " << bestMove.ToAlgebraicNotation() << std::endl;
      board.MakeMove(bestMove, currentPlayer);
    }

    // Switch the current player for the next turn
    currentPlayer = (currentPlayer == WHITE) ? BLACK : WHITE;
  }

  return 0;
}

