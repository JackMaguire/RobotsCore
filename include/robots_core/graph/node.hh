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
  
  count, //these should stay at the end
  NONE = count //these should stay at the end
};

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
    if( b.cell(p) == Occupant::HUMAN ){
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
      RC_DEBUG_ASSERT( false );
    }
  };

  int dy() const {
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
      RC_DEBUG_ASSERT( false );
    }
  };
};

/*std::vector< Node >
determine_nodes_for_graph(
  RobotsGame const & game,
  unsigned int const max_nodes
){
  //TODO
}*/

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

std::vector< Node >
get_all_nodes(
  Board const & b
){
  std::vector< Node > nodes;
  nodes.reserve( MAX_N_ROBOTS + 9 + 4 );

  Position const h = b.human_position();
  using P = Position;

  nodes.emplace_back( b, P{ h.x, Board::HEIGHT }, SpecialCaseNode::TOP_OOB );
  nodes.emplace_back( b, P{ h.x, -1 }, SpecialCaseNode::BOTTOM_OOB );
  nodes.emplace_back( b, P{ -1, h.y }, SpecialCaseNode::LEFT_OOB );
  nodes.emplace_back( b, P{ Board::WIDTH, h.y }, SpecialCaseNode::RIGHT_OOB );

  for( sm_int x = 0; x < Board::WIDTH; ++x ){
    for( sm_int y = 0; y < Board::HEIGHT; ++y ){
      Position const p{ x, y };

      if( p.distance_sqaured( h ) < 4 ){
	nodes.emplace_back( b, p, get_direction( h, p ) );
      } else if( b.cell(p) != Occupant::EMPTY ){
	nodes.emplace_back( b, p );
      }
    }
  }

  return nodes;
}

}
}
