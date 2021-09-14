#include <robots_core/board.hh>

#include <iostream>
#include <cassert>
#include <algorithm>

namespace robots_core {
namespace tests {

//g++ board.test.cc -o board.test -I../ -std=c++11

struct BoardTests {
  static void run(){

    Board const b( "300000000000000000000000000000000000000000000000000000000000010000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000010000000000000000010000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000100000000000000000000000000000000000000000000000000000000000000000000000010000000001000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000200000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000100000000000000000000000000000000000000000000100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001000000000000000000000000000000000000000000000000000001000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000" );

    bool const passed = test_deserialization( b )
      && test_cell_is_safe_for_teleport( b );

    if( passed ) std::cout << "PASSED BoardTests" << std::endl;
    else         std::cout << "FAILED BoardTests" << std::endl;
  }

  static bool test_deserialization( Board const & b ){
    assert( b.n_robots() == 10 );
    assert( b.human_position().x == 23 );
    assert( b.human_position().y == 15 );

    Board::PositionVec expected_robots_positions({
	{2, 1},
	{7, 4},
	{7, 22},
	{15, 24},
	{18, 17},
	{18, 7},
	{26, 24},
	{28, 9},
	{34, 17},
	{36, 11}
      });

    assert( expected_robots_positions.size() == b.robots().size() );

    for( Position const & ep : expected_robots_positions ){
      auto iter = std::find( b.robots().begin(), b.robots().end(), ep );
      //auto iter = std::ranges::find( b.robots(), ep );
      assert( iter != b.robots().end() );  
    }

    for( sm_int i = 0; i < Board::WIDTH; ++i ){
      for( sm_int j = 0; j < Board::HEIGHT; ++j ){
	Position const p({i,j});

	assert( b.cell( i, j ) == b.cell( p ) );

	bool const should_be_robot = std::find( expected_robots_positions.begin(), expected_robots_positions.end(), p ) != expected_robots_positions.end();

	if( should_be_robot ){
	  assert( b.cell( p ) == Occupant::ROBOT );
	} else if( i == 23 and j == 15 ){
	  assert( b.cell( p ) == Occupant::HUMAN );
	} else if( i == 0 and j == 0 ){
	  assert( b.cell( p ) == Occupant::FIRE );
	} else {
	  assert( b.cell( p ) == Occupant::EMPTY );
	}
      }
    }

    return true;
  }

  static bool test_cell_is_safe_for_teleport( Board const & b ){

    // ==true and ==false are okay here
    assert( b.cell_is_safe_for_teleport({ 0, 0 }) == false ); //fire
    assert( b.cell_is_safe_for_teleport({ 0, 1 }) == true  );

    assert( b.cell_is_safe_for_teleport({ 23, 15 }) == true  ); //human pos, not near robot

    assert( b.cell_is_safe_for_teleport({ 18, 17 }) == false ); //robot pos
    assert( b.cell_is_safe_for_teleport({ 18, 18 }) == false ); //next to robot pos

    return true;
  }


};

} //tests
} //robots_core
