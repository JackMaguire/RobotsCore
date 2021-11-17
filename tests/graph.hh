#include <robots_core/board.hh>
#include <robots_core/graph/node.hh>
#include <robots_core/asserts.hh>

#include <iostream>
#include <cassert>
#include <algorithm>
#include <stdexcept>

namespace robots_core {
namespace tests {

using namespace robots_core::graph;

constexpr double pi = 3.14159265358979323846;

struct GraphTests {

  static void run(){
    bool const passed = test_angle_calc()
      && test_angle_calc()
      ;

    if( passed ) std::cout << "PASSED GraphTests" << std::endl;
    else         std::cout << "FAILED GraphTests" << std::endl;
  }

  static bool test_angle_calc(){
    Board const b;
    Position const h = b.human_position();
    using P = Position;

    auto && test_delta =
      [&]( P const delta, double const expected ){
	RC_ASSERT_DELTA( calc_angle({0,0}, delta), expected, 0.001 );
	RC_ASSERT_DELTA( determine_orientation( b, h-delta ), expected, 0.001 );
      };

    //0 degrees
    test_delta( P{1,0}, 0 ); //0 degrees
    RC_ASSERT( calc_angle({0,0}, {1,0}) == 0 );
    RC_ASSERT_DELTA( determine_orientation( b, h-P{1,0} ), 0, 0.001 );

    //45 degrees
    RC_ASSERT_DELTA( calc_angle({0,0}, {1,1}), pi/4, 0.001 );
    RC_ASSERT_DELTA( determine_orientation( b, h-P{1,1} ), pi/4, 0.001 );

    //90 degrees
    RC_ASSERT_DELTA( calc_angle({0,0}, {0,1}), pi/2, 0.001 );

    //135 degrees
    RC_ASSERT_DELTA( calc_angle({0,0}, {-1,1}), 3*pi/4, 0.001 );

    //180 degrees
    RC_ASSERT_DELTA( calc_angle({0,0}, {-1,0}), pi, 0.001 );

    //-45 degrees
    RC_ASSERT_DELTA( calc_angle({0,0}, {1,-1}), -pi/4, 0.001 );

    //-90 degrees
    RC_ASSERT_DELTA( calc_angle({0,0}, {0,-1}), -pi/2, 0.001 );

    //-135 degrees
    RC_ASSERT_DELTA( calc_angle({0,0}, {-1,-1}), -3*pi/4, 0.001 );

    return true;
  }
};

} //tests
} //robots_core
