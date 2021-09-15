#include <robots_core/board.hh>

#include <iostream>
#include <cassert>
#include <algorithm>

namespace robots_core {
namespace tests {

struct XTests {

  static void run(){
    bool const passed = test()
      && test()
      ;

    if( passed ) std::cout << "PASSED XTests" << std::endl;
    else         std::cout << "FAILED XTests" << std::endl;
  }

  static bool test(){
    return true;
  }

};

} //tests
} //robots_core
