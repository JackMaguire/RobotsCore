//g++ unit_tests.cc -o unit_tests -O3 -Wall -Wextra -Iinclude -std=c++17 -Wshadow

#include "tests/board.hh" 

int main(){
  using namespace robots_core::tests;
  BoardTests::run();
}
