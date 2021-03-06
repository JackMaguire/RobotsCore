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

constexpr static unsigned char F = NOccupantTypes + 5;
//Extra 3 elements are for possible moves
//+0: if legal move, ln( n_robots_killed ), -1 if n_robots_killed=0
//+1: if legal and cascade safe
//+2: number of safe teleports left
//+3: ln( number of robots left )
//+4: 1 if legal move, 0 otherwise

constexpr static unsigned char S = 8;
//TRANSLATION
//0: ln( distance )-1
//1: ln( small_leg_dist )-1 // smaller cathetus of right triangle
//2: ln( long_leg_dist )-1  //  longer cathetus of right triangle
//3: ratio of small_leg_dist / long_leg_dist
//4: sin( trans angle )
//5: cos( trans angle )

//ROTATION
//6: sin( rot angle )
//7: cos( rot angle )

constexpr static bool edges_are_symmetric = false;

//Checks occupant type and distance to determine if we want to represent this pairing with an edge
static
bool
edge_should_exist(
  Node const & i,
  Node const & j,
  Board const & b
);

static
bool
edge_should_exist(
  Node const & i,
  Node const & j,
  Board const & b,
  AllForecastResults const & fcasts
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



namespace {
template< typename MoveLawyer >
bool
edge_should_exist_tmpl(
  Node const & i,
  Node const & j,
  Board const & b,
  MoveLawyer const & lawyer
){
  //1. TELEPORT
  if( i.special_case == SpecialCaseNode::TELEPORT ){
    return special_case_is_move( j.special_case ) and lawyer.is_legal( j );
  }
  if( j.special_case == SpecialCaseNode::TELEPORT ){
    return special_case_is_move( i.special_case ) and lawyer.is_legal( i );
  }

  //2. SPECIAL CASES
  if( i.special_case != SpecialCaseNode::NONE
    or j.special_case != SpecialCaseNode::NONE ){
    //Edges for all things if they are special
    return true;
  }

  return false; //!!!

  //3. TWO ROBOTS THAT CAN COLLIDE
  Occupant const occ_i = b.cell( i.position );
  Occupant const occ_j = b.cell( j.position );
  if( occ_i == Occupant::ROBOT and occ_j == Occupant::ROBOT ){
    if( i.position.x == j.position.x or i.position.y == j.position.y ){
      return i.position.distance( j.position ) < 10; //relatively arbitrary
    }
  }

  //4. TWO ROBOTS/EXPLOSIONS NOT IN LINE
  return i.position.distance( j.position ) < 5; //relatively arbitrary
}
}

bool
GraphDecorator::edge_should_exist(
  Node const & i,
  Node const & j,
  Board const & b
){
  class IsLegal {
    Board const * board_;

  public:
    IsLegal( Board const * b ):
      board_( b )
    {}

    bool is_legal( Node const & n ) const {
      //can assume n is a move
      return n.calc_is_legal_move( *board_ );
    }
  };

  IsLegal const lawyer( &b );

  return edge_should_exist_tmpl( i, j, b, lawyer );
}

bool
GraphDecorator::edge_should_exist(
  Node const & i,
  Node const & j,
  Board const & b,
  AllForecastResults const & fcasts
){
  class IsLegal {
    AllForecastResults const * fcasts_;

  public:
    IsLegal( AllForecastResults const * fcasts ):
      fcasts_( fcasts )
    {}

    bool is_legal( Node const & n ) const {
      //can assume n is a move
      return (*fcasts_)[ n.dx()+1 ][ n.dy()+1 ].legal;
    }
  };

  IsLegal const lawyer( &fcasts );

  return edge_should_exist_tmpl( i, j, b, lawyer );
}

std::array< float, GraphDecorator::F >
GraphDecorator::calculate_node(
  Node const & node,
  RobotsGame const & game
) {
  Board const & b = game.board();

  std::array< float, F > data;
  data.fill( 0 );

  Occupant const occ = cell_or_oob( b, node.position );
  data[ int(occ) ] = 1.0;

  constexpr unsigned int offset( NOccupantTypes );
  constexpr unsigned int LEGAL = offset+4;

  if( special_case_is_move( node.special_case ) ){
    ForecastResults const forecast = forecast_move( b, node.dx(), node.dy() );
    if( forecast.legal ){
      if( forecast.robots_killed == 0 ){
	data[ offset ] = -1;
      } else {
	data[ offset ] = log( float(forecast.robots_killed) );
      }

      if( forecast.cascade_safe ){
	data[ offset+1 ] = 1.0;
      }

      data[ offset+2 ] = game.n_safe_teleports_remaining();
      data[ offset+3 ] = log( game.board().n_robots() );
      data[ LEGAL ] = 1.0;
    }// if forecast.legal
  } else if( node.special_case == SpecialCaseNode::TELEPORT ){
    data[ offset+2 ] = game.n_safe_teleports_remaining();
    data[ offset+3 ] = log( game.board().n_robots() );
    data[ LEGAL ] = 1.0;
  }

#ifdef RC_DEBUG
  for( float const f : data ){
    RC_DEBUG_ASSERT( not std::isnan( f ) );
    RC_DEBUG_ASSERT( not std::isinf( f ) );
  }
#endif

  return data;
}

std::array< float, GraphDecorator::S >
GraphDecorator::calculate_edge(
  Node const & i,
  Node const & j
) {
  std::array< float, S > data;
  data.fill( 0.0 );

  if( i.special_case == SpecialCaseNode::TELEPORT or
    j.special_case == SpecialCaseNode::TELEPORT ){
    data.fill( -2.0 );
    return data;
  }

  //TRANSLATION
  RC_DEBUG_ASSERT( i.position != j.position );
  float const distance = i.position.distance( j.position );
  data[ 0 ] = log( distance ) - 1;

  sm_int const x_dist = abs( i.position.x - j.position.x );
  sm_int const y_dist = abs( i.position.y - j.position.y );

  if( x_dist < y_dist ){
    data[ 1 ] = log( x_dist+1 ) - 1;
    data[ 2 ] = log( y_dist+1 ) - 1;
    data[ 3 ] = float( x_dist ) / float( y_dist );
  } else {
    data[ 1 ] = log( y_dist+1 ) - 1;
    data[ 2 ] = log( x_dist+1 ) - 1;
    data[ 3 ] = float( y_dist ) / float( x_dist );
  }

  double const abs_angle_to_j = calc_angle( i.position, j.position );
  double const rel_angle_to_j = abs_angle_to_j - i.orientation;
  data[ 4 ] = sin( rel_angle_to_j );
  data[ 5 ] = cos( rel_angle_to_j );

  //ROTATION
  double const angle_from_i_to_j_rad = j.orientation - i.orientation;
  data[ 6 ] = sin( angle_from_i_to_j_rad );
  data[ 7 ] = cos( angle_from_i_to_j_rad );

#ifdef RC_DEBUG
  for( float const f : data ){
    RC_DEBUG_ASSERT( not std::isnan( f ) );
    RC_DEBUG_ASSERT( not std::isinf( f ) );
  }
#endif
  //std::cout << angle_from_i_to_j_rad << " " << data[ 6 ] << " " << data[ 7 ] << std::endl;
  //TODO check for rows of all zeros
  
  return data;
}

}
}
