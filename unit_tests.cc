//g++ unit_tests.cc -o unit_tests -O3 -Wall -Wextra -Iinclude -std=c++17 -Wshadow

//keep this first!
#define RC_TESTS

//Unit Tests:
#include "tests/board.hh" 
#include "tests/forecasting.hh" 
#include "tests/game.hh" 
#include "tests/pocket.hh" 
#include "tests/graph.hh" 

//Special Test(s):
#include "tests/test_compile.hh"

int main(){
  using namespace robots_core::tests;

  //Top-Level
  BoardTests::run();
  ForecastingTests::run();
  GameTests::run();
  PocketTests::run();
  GraphTests::run();
}
