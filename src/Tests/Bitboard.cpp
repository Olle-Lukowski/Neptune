#include "Neptune/Bitboard.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Bitboard Operations") {
  Bitboard bitboard;

  SECTION("Setting and clearing bits") {
    bitboard.SetBit(0);
    REQUIRE(bitboard.IsSet(0) == true);

    bitboard.ClearBit(0);
    REQUIRE(bitboard.IsSet(0) == false);
  }

  SECTION("Getting and setting underlying board") {
    bitboard.SetBoard(0x55AA55AA55AA55AAULL);
    REQUIRE(bitboard.GetBoard() == 0x55AA55AA55AA55AAULL);
  }
}
