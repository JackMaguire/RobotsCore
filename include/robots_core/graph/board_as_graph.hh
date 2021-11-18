#pragma once

#include <robots_core/game.hh>
#include <robots_core/forecasting.hh>
#include <robots_core/asserts.hh>

#include <robots_core/graph/node.hh>

//#include <array>
#include <cmath>  //log, sin, cos

namespace robots_core{
namespace graph{

class GraphDecorator {
public:

constexpr static unsigned char F = NOccupantTypes + 3;
//Extra 3 elements are for possible moves
//+0: if legal move, ln( n_robots_killed ), -1 if n_robots_killed=0
//+1: number of safe teleports left
//+2: 1 if legal move, 0 otherwise

constexpr static unsigned char S = 6;
//0: ln( distance )-1
//1: ln( small_leg_dist )-1 // smaller cathetus of right triangle
//2: ln( long_leg_dist )-1  //  longer cathetus of right triangle
//3: ratio of small_leg_dist / long_leg_dist
//4: sin( d angle )
//5: cos( d angle )

constexpr static bool edges_are_symmetric = false;

//Checks occupant type and distance to determine if we want to represent this pairing with an edge
static
bool
edge_should_exist(
  Node const & i,
  Node const & j,
  Board const & b
);

public: //Generating Data

static
std::array< float, F >
calculate_node(
  Node const & node,
  RobotsGame const & game
);

static
std::array< float, S >
calculate_edge(
  Node const & i,
  Node const & j
);
  
}; // class GraphDecorator

bool
GraphDecorator::edge_should_exist(
  Node const & i,
  Node const & j,
  Board const & b
){
  //1. SPECIAL CASES
  if( i.special_case != SpecialCaseNode::NONE
    or j.special_case != SpecialCaseNode::NONE ){
    //Edges for all things if they are special
    return true;
  }

  //2. TWO ROBOTS THAT CAN COLLIDE
  Occupant const occ_i = b.cell( i.position );
  Occupant const occ_j = b.cell( j.position );
  if( occ_i == Occupant::ROBOT and occ_j == Occupant::ROBOT ){
    if( i.position.x == j.position.x or i.position.y == j.position.y ){
      return true;
    }
  }

  //3. TWO ROBOTS/EXPLOSIONS NOT IN LINE
  return i.position.distance( j.position ) < 10; //relatively arbitrary
}



std::array< float, GraphDecorator::F >
GraphDecorator::calculate_node(
  Node const & node,
  RobotsGame const & game
) {
  Board const & b = game.board();

  std::array< float, F > data;
  data.fill( 0 );

  Occupant const occ = b.cell( node.position );
  data[ int(occ) ] = 1.0;

  constexpr unsigned int offset( NOccupantTypes );

  if( special_case_is_move( node.special_case ) ){
    ForecastResults const forecast = forecast_move( b, node.dx(), node.dy() );
    if( forecast.legal ){
      if( forecast.robots_killed == 0 ){
	data[ offset ] = -1;
      } else {
	data[ offset ] = log( float(forecast.robots_killed) );
      }
      data[ offset+1 ] = game.n_safe_teleports_remaining();

      data[ offset+2 ] = 1.0;
    }// if forecast.legal
  }

  return data;
}

std::array< float, GraphDecorator::S >
GraphDecorator::calculate_edge(
  Node const & i,
  Node const & j
) {
  std::array< float, S > data;
  //data.fill( 0 );

  float const distance = i.position.distance( j.position );
  data[ 0 ] = log( distance ) - 1;

  sm_int const x_dist = abs( i.position.x - j.position.x );
  sm_int const y_dist = abs( i.position.y - j.position.y );

  if( x_dist < y_dist ){
    data[ 1 ] = log( x_dist ) - 1;
    data[ 2 ] = log( y_dist ) - 1;
    data[ 3 ] = float( x_dist ) / float( y_dist );
  } else {
    data[ 1 ] = log( y_dist ) - 1;
    data[ 2 ] = log( x_dist ) - 1;
    data[ 3 ] = float( y_dist ) / float( x_dist );
  }

  double const angle_from_i_to_j_rad = j.orientation - i.orientation;
  data[ 4 ] = sin( angle_from_i_to_j_rad );
  data[ 5 ] = cos( angle_from_i_to_j_rad );

  return data;
}

}
}
