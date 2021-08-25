#pragma once

#include <robots_core/game.hh>
#include <robots_core/forecasting.hh>

#include <robots_core/graph/node.hh>

//#include <array>
#include <cassert>
#include <cmath>  //log

namespace robots_core{
namespace graph{

class GraphDecorator {
public:

constexpr static unsigned char F = NOccupantTypes + 3;
//Extra 3 elements are for possible moves
//+0: 1 if legal move, 0 otherwise
//+1: if legal move, ln( n_robots_killed ), -1 if n_robots_killed=0
//+2: number of safe teleports left

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

static
std::array< float, F >
calculate_node(
  Node const & node,
  Game const & game
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

}; // class GraphDecorator

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

std::array< float, F >
GraphDecorator::calculate_node(
  Node const & node,
  Game const & game
) {
  Board const & b = game.board();

  std::array< float, F > data;
  data.fill( 0 );

  Occupant const occ = b.cell( node.position );
  data[ int(occ) ] = 1.0;

  constexpr unsigned int offset( NOccupantTypes );

  if( occ == Occupant::HUMAN or occ == Occupant::OOB ){
    ForecastResults const forecast = forecast_move( b, node.dx(), node.dy() );
    if( forecast.legal ){
      data[ offset ] = 1.0;
      if( forecast.robots_killed == 0 ){
	data[ offset+1 ] = -1;
      } else {
	data[ offset+1 ] = log( float(forecast.robots_killed) );
      }
      data[ offset+2 ] = game.n_safe_teleports_remaining();
    }// if forecast.legal
  }

  return data;
}

}
}
