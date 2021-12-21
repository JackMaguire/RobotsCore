#include <iostream>

#include <robots_core/game.hh>
#include <robots_core/visualization.hh>

//Example: g++ test_vis.cc -o test_vis -O3 -Wall -Wextra -Iinclude -std=c++17

int main(){
  robots_core::Board b;
  std::cout << robots_core::to_svg_string( b ) << std::endl;
}
