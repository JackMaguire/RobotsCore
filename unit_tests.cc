//g++ unit_tests.cc -o unit_tests -O3 -Wall -Wextra -Iinclude -std=c++17 -Wshadow

//keep this first!
#define RC_TESTS

#include "tests/board.hh" 

int main(){
  using namespace robots_core::tests;
  BoardTests::run();
}
