#include <iostream>

#include <robots_core/game.hh>
#include <robots_core/forecasting.hh>
#include <robots_core/strategy/recursion.hh>
#include <robots_core/strategy/stall.hh>
#include <robots_core/graph/board_as_graph.hh>
#include <robots_core/graph/dense.hh>

#include <robots_core/asserts.hh>

//Example: g++ test_compile.cc -o test_compile -O3 -Wall -Wextra -Iinclude -std=c++17

int main(){
  robots_core::Board b;
  std::cout << b.get_stringified_representation() << std::endl;
}
