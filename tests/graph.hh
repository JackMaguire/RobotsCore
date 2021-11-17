#include <robots_core/board.hh>
#include <robots_core/graph/node.hh>
#include <robots_core/asserts.hh>

#include <iostream>
#include <cassert>
#include <algorithm>
#include <stdexcept>

namespace robots_core {
namespace tests {

struct GraphTests {

  static void run(){
    bool const passed = test_angle_calc()
      && test_angle_calc()
      ;

    if( passed ) std::cout << "PASSED GraphTests" << std::endl;
    else         std::cout << "FAILED GraphTests" << std::endl;
  }

  static bool test_angle_calc(){
    RC_ASSERT( calc_angle({0,0}, {1,0}) == 0 );
  }
};

} //tests
} //robots_core
