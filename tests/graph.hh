#include <robots_core/board.hh>
#include <robots_core/graph/node.hh>
#include <robots_core/graph/dense.hh>
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
      && test_node_vec_no_explosions()
      //&& test_node_vec_three_explosions()
      && test_build_graph()
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
    test_delta( P{ 1, 0 }, 0       ); //0 degrees
    test_delta( P{ 1, 1 }, pi/4    ); //45 degrees
    test_delta( P{ 0, 1 }, pi/2    ); //90 degrees
    test_delta( P{-1, 1 }, 3*pi/4  ); //135 degrees
    test_delta( P{-1, 0 }, pi      ); //180 degrees
    test_delta( P{ 1,-1 }, -pi/4   ); //-45 degrees
    test_delta( P{ 0,-1 }, -pi/2   ); //-90 degrees
    test_delta( P{-1,-1 }, -3*pi/4 ); //-135 degrees

    RC_ASSERT_DELTA( determine_orientation( b, h ), -pi/2, 0.001 );
    
    return true;
  }

  static bool test_node_vec_no_explosions(){
    Board const b("000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000010000000000000000000000000000000000000000000000000000000000010000000000010000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000200100000000000100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000010000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000010000000000000000000000000000000000000000000000000000000000000");

    //Position const h = b.human_position();
    // using P = Position;

    auto const nodes = get_all_nodes( b );
    RC_ASSERT_EQUALS( nodes.size(), 9+4+b.n_robots_uint() );
    
    std::array< SpecialCaseNode, 9 > const order_that_we_expect{
      SpecialCaseNode::Z,
      SpecialCaseNode::A,
      SpecialCaseNode::Q,
      SpecialCaseNode::X,
      SpecialCaseNode::S,
      SpecialCaseNode::W,
      SpecialCaseNode::C,
      SpecialCaseNode::D,
      SpecialCaseNode::E
    };

    
    {
      auto special_nodes = nodes;
      special_nodes.erase(
	std::remove_if(
	  special_nodes.begin(), 
	  special_nodes.end(),
	  [](Node x) { return x.special_case == SpecialCaseNode::NONE; }
	),
	special_nodes.end()
      );
      //std::erase_if(special_nodes, [](Node x) { return x.special_case == SpecialCaseNode::NONE; });
      
      for( int i = 0; i < 9; ++i ){
	//std::cout << i << " " << int(special_nodes[4+i].special_case) << std::endl;
	RC_ASSERT_EQUALS(int(special_nodes[4+i].special_case), int(order_that_we_expect[i]));
      }
    }

    return true;
  }

  static bool test_node_vec_three_explosions(){
    Board const b("000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000300000000000000000000000000000000000000000000000000000000000000000100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000300000000000000000000000000000000000000000000000000000000000000000000000000000000010000000000000000000000000000000000000000000000000000000000010000000000010000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000200100000000000100000000000000000000000000000000000000030000000000000000000000000000000000000000000000000000000000000000000000000000001000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000010000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000010000000000000000000000000000000000000000000000000000000000000");

    //Position const h = b.human_position();
    // using P = Position;

    auto const nodes = get_all_nodes( b );
    RC_ASSERT_EQUALS( nodes.size(), 9+4+b.n_robots_uint()+3 );
    
    std::array< SpecialCaseNode, 9 > const order_that_we_expect{
      SpecialCaseNode::Z,
      SpecialCaseNode::A,
      SpecialCaseNode::Q,
      SpecialCaseNode::X,
      SpecialCaseNode::S,
      SpecialCaseNode::W,
      SpecialCaseNode::C,
      SpecialCaseNode::D,
      SpecialCaseNode::E
    };

    
    {
      auto special_nodes = nodes;
      special_nodes.erase(
	std::remove_if(
	  special_nodes.begin(), 
	  special_nodes.end(),
	  [](Node x) { return x.special_case == SpecialCaseNode::NONE; }
	),
	special_nodes.end()
      );
      //std::erase_if(special_nodes, [](Node x) { return x.special_case == SpecialCaseNode::NONE; });
      
      for( int i = 0; i < 9; ++i ){
	//std::cout << i << " " << int(special_nodes[4+i].special_case) << std::endl;
	RC_ASSERT_EQUALS(int(special_nodes[4+i].special_case), int(order_that_we_expect[i]));
      }
    }

    return true;
  }

  static bool test_build_graph(){
    RobotsGame const g;

    DenseGraph const dg( g );
    RC_ASSERT_EQUALS( dg.cached_nodes.size(), dg.x.size() );
    RC_ASSERT_EQUALS( dg.cached_nodes.size(), dg.a.size() );
    RC_ASSERT_EQUALS( dg.cached_nodes.size(), dg.a[0].size() );
    RC_ASSERT_EQUALS( dg.cached_nodes.size(), dg.e.size() );
    RC_ASSERT_EQUALS( dg.cached_nodes.size(), dg.e[0].size() );
    
    return true;
  }


};

} //tests
} //robots_core
