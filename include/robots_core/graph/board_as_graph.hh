#pragma once

#include <robots_core/board.hh>

//#include <array>
#include <cassert>

namespace robots_core{
namespace graph{

enum class SpecialCaseNodes {
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
  none = count //these should stay at the end
};


struct Node {
  Position position;
  SpecialCaseNodes special_case = SpecialCaseNodes::none;
};

class GraphDecorator {
public:

constexpr static unsigned char F = NOccupantTypes + 3;
//Extra 3 elements are for possible moves
//+1: 1 if legal move, 0 otherwise
//+2: if legal move, ln( n_robots_killed ), -1 if n_robots_killed=0
//+3: number of safe teleports left

constexpr static unsigned char S = 4;
//0: ln( distance )-1
//1: ln( small_leg_dist )-1 // smaller cathetus of right triangle
//2: ln( long_leg_dist )-1  //  longer cathetus of right triangle
//3: ratio of small_leg_dist / long_leg_dist

//Checks occupant type and distance to determine if we want to represent this pairing with an edge
static
bool
edge_should_exist(
  Node const & i,
  Node const & j,
  Board const & b
);

  
private:

//This override is private because distances are calculated in special ways for OOB Nodes
static
bool
edge_should_exist(
  Occupant const occA,
  Occupant const occB,
  float const distance
);

};

bool
GraphDecorator::edge_should_exist(
  Node const & i,
  Node const & j,
  Board const & b
){
  Occupant const occ_i = b.cell( i.position );
  Occupant const occ_j = b.cell( j.position );

  float distance = 0.0;

  if( occ_i == Occupant::OOB ){
    if( occ_j == Occupant::OOB ) return false;

    switch( i.special_case ){
    case SpecialCaseNodes::LEFT_OOB:   distance = 1 +             j.position.x; break;
    case SpecialCaseNodes::RIGHT_OOB:  distance = Board::WIDTH -  j.position.x; break;
    case SpecialCaseNodes::BOTTOM_OOB: distance = 1 +             j.position.y; break;
    case SpecialCaseNodes::TOP_OOB:    distance = Board::HEIGHT - j.position.y; break;
    default: assert( false ); break;
    }
  } else if ( occ_j == Occupant::OOB ){
    switch( j.special_case ){
    case SpecialCaseNodes::LEFT_OOB:   distance = 1 +             i.position.x; break;
    case SpecialCaseNodes::RIGHT_OOB:  distance = Board::WIDTH  - i.position.x; break;
    case SpecialCaseNodes::BOTTOM_OOB: distance = 1 +             i.position.y; break;
    case SpecialCaseNodes::TOP_OOB:    distance = Board::HEIGHT - i.position.y; break;
    default: assert( false ); break;
    }
  } else {
    distance = i.position.distance( j.position );
  }

  return edge_should_exist( occ_i, occ_j, distance );
}


bool
GraphDecorator::edge_should_exist(
  Occupant const occA,
  Occupant const occB,
  float const distance
){
  switch( occA ){
  case Occupant::HUMAN:
  case Occupant::EMPTY:
    return true;
  case Occupant::OOB:
    if( occB == Occupant::OOB) return false;
  default:
    break;
  }

  switch( occB ){
  case Occupant::HUMAN:
  case Occupant::EMPTY:
    return true;
  default:
    break;
  }

  return distance < 10;

}


}
}
