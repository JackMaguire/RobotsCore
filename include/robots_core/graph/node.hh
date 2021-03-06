#pragma once

#include <robots_core/game.hh>
#include <robots_core/forecasting.hh>
#include <robots_core/asserts.hh>

//#include <array>
#include <cmath>  //log, atan2
#include <vector>

namespace robots_core{
namespace graph{

enum class SpecialCaseNode : unsigned char {
  Q = 0,
  W,
  E,
  A,
  S,
  D,
  Z,
  X,
  C,
  LEFT_OOB,
  TOP_OOB,
  RIGHT_OOB,
  BOTTOM_OOB,

  TELEPORT,
  
  count, //these should stay at the end
  NONE = count //these should stay at the end
};

bool
special_case_is_move(
  SpecialCaseNode const n
){
  static_assert( int(SpecialCaseNode::Q) < 9, "SpecialCaseNode order" );
  static_assert( int(SpecialCaseNode::W) < 9, "SpecialCaseNode order" );
  static_assert( int(SpecialCaseNode::E) < 9, "SpecialCaseNode order" );
  static_assert( int(SpecialCaseNode::A) < 9, "SpecialCaseNode order" );
  static_assert( int(SpecialCaseNode::S) < 9, "SpecialCaseNode order" );
  static_assert( int(SpecialCaseNode::D) < 9, "SpecialCaseNode order" );
  static_assert( int(SpecialCaseNode::Z) < 9, "SpecialCaseNode order" );
  static_assert( int(SpecialCaseNode::X) < 9, "SpecialCaseNode order" );
  static_assert( int(SpecialCaseNode::C) < 9, "SpecialCaseNode order" );
  return int(n) < 9;
}

double calc_angle(
  Position const & p1,
  Position const & p3
){
  Position const p2 = p1 + Position({10,0});

  //https://stackoverflow.com/questions/1211212/how-to-calculate-an-angle-from-three-points
  double const result =
    atan2(p3.y - p1.y, p3.x - p1.x) -
    atan2(p2.y - p1.y, p2.x - p1.x);

  return result;
}

double determine_orientation(
  Board const & b,
  Position const p
){
  if( b.position_is_in_bounds(p) ){
    if( b.cell(p) == Occupant::HUMAN or b.cell(p) == Occupant::EMPTY ){
      //includes TELEPORT!

      //Destination position is { 22, 14.5 }
      //but we need these as integers.
      //let's change units by doubling all numbers
      //Source is p*2, destination is {44, 29}

      Position const source = p*2;
      return calc_angle( source, Position({44, 29}) );
    }
  }

  //This cell is looking directly at the human
  return calc_angle( p, b.human_position() );

}

Occupant
cell_or_oob( Board const & b, Position const p ){
  if( b.position_is_in_bounds(p) ) return b.cell(p);
  else return Occupant::OOB;
}

int
dx_for_node( SpecialCaseNode const special_case ) {
  switch( special_case ){
  case SpecialCaseNode::Q:
  case SpecialCaseNode::A:
  case SpecialCaseNode::Z:
    return -1;
  case SpecialCaseNode::W:
  case SpecialCaseNode::S:
  case SpecialCaseNode::X:
    return 0;
  case SpecialCaseNode::E:
  case SpecialCaseNode::D:
  case SpecialCaseNode::C:
    return 1;
  default:
    RC_ASSERT( false );
    return 2;
  }
};

int
dy_for_node( SpecialCaseNode const special_case ) {
  switch( special_case ){
  case SpecialCaseNode::Z:
  case SpecialCaseNode::X:
  case SpecialCaseNode::C:
    return -1;
  case SpecialCaseNode::A:
  case SpecialCaseNode::S:
  case SpecialCaseNode::D:
    return 0;
  case SpecialCaseNode::Q:
  case SpecialCaseNode::W:
  case SpecialCaseNode::E:
    return 1;
  default:
    std::cout << "FAILED " << int(special_case) << std::endl;
    RC_ASSERT( false );
    return 2;
  }
};

struct Node {
  Occupant occupant;
  Position position;
  SpecialCaseNode special_case = SpecialCaseNode::NONE;
  double orientation;
  
  Node(
    Board const & b,
    Position const p,
    SpecialCaseNode spcase = SpecialCaseNode::NONE
  ):
    occupant( cell_or_oob( b, p ) ),
    position( p ),
    special_case( spcase ),
    orientation( determine_orientation( b, p ) )
  {}

  int dx() const {
    return dx_for_node( special_case );
  };

  int dy() const {
    return dy_for_node( special_case );
  };

  bool
  calc_is_legal_move( Board const & b ) const {
    return forecast_move( b, dx(), dy() ).legal;
  }
};

SpecialCaseNode
get_direction(
  Position const h,
  Position const p
){
  if( p.x == h.x-1 ){
    if( p.y == h.y-1 ) return SpecialCaseNode::Z;
    else if( p.y == h.y ) return SpecialCaseNode::A;
    else if( p.y == h.y+1 ) return SpecialCaseNode::Q;
    else return SpecialCaseNode::NONE;
  }

  if( p.x == h.x ){
    if( p.y == h.y-1 ) return SpecialCaseNode::X;
    else if( p.y == h.y ) return SpecialCaseNode::S;
    else if( p.y == h.y+1 ) return SpecialCaseNode::W;
    else return SpecialCaseNode::NONE;
  }

  if( p.x == h.x+1 ){
    if( p.y == h.y-1 ) return SpecialCaseNode::C;
    else if( p.y == h.y ) return SpecialCaseNode::D;
    else if( p.y == h.y+1 ) return SpecialCaseNode::E;
    else return SpecialCaseNode::NONE;
  }

  return SpecialCaseNode::NONE;
}

bool
is_adjacent9( Position const & p1, Position const & p2 ) {
  return std::abs( p1.x - p2.x ) <= 1
    and  std::abs( p1.y - p2.y ) <= 1;
}


std::vector< Node >
get_all_nodes(
  Board const & b
){
  std::vector< Node > nodes;
  nodes.reserve( MAX_N_ROBOTS + 9 + 4 );

  Position const h = b.human_position();
  using P = Position;

  nodes.emplace_back( b, h, SpecialCaseNode::TELEPORT );

  nodes.emplace_back( b, P{ h.x, Board::HEIGHT }, SpecialCaseNode::TOP_OOB );
  nodes.emplace_back( b, P{ h.x, -1 }, SpecialCaseNode::BOTTOM_OOB );
  nodes.emplace_back( b, P{ -1, h.y }, SpecialCaseNode::LEFT_OOB );
  nodes.emplace_back( b, P{ Board::WIDTH, h.y }, SpecialCaseNode::RIGHT_OOB );

  for( sm_int x = 0; x < Board::WIDTH; ++x ){
    for( sm_int y = 0; y < Board::HEIGHT; ++y ){
      Position const p{ x, y };

      if( is_adjacent9( h, p ) ){
	Node const n( b, p, get_direction( h, p ) );
	if( n.calc_is_legal_move( b ) ){
	  nodes.push_back( n );
	}
      } else if( b.cell(p) != Occupant::EMPTY ){
	nodes.emplace_back( b, p );
      }
    }
  }

  return nodes;
}

}
}
