#pragma once

#include <robots_core/game.hh>
#include <robots_core/forecasting.hh>
#include <robots_core/asserts.hh>

#include <robots_core/graph/board_as_graph.hh>
#include <robots_core/graph/node.hh>

//#include <array>
#include <cmath>  //log

#include <vector>
#include <array>

namespace robots_core{
namespace graph{

struct DenseGraph {
  
  using X = std::vector< std::array< float, GraphDecorator::F > >;
  using A = std::vector< std::vector< unsigned short int > >;
  using E = std::vector< std::vector< std::array< float, GraphDecorator::S > > >;

  X x;
  A a;
  E e;
  std::vector< Node > cached_nodes;

  DenseGraph(){}

  DenseGraph(
    RobotsGame const & game
  ){
    construct( game );
  }

  DenseGraph(
    RobotsGame const & game,
    std::vector< Node > const & nodes
  ){
    construct( game, nodes ); //construct calls clear
  }

  void
  reset( unsigned int const n_nodes ){ //zero initialize

    // X
    x.resize( n_nodes );
    for( auto & vec : x ) vec.fill( 0.0 );

    // A
    a.resize( n_nodes );
    for( auto & vec : a ){
      vec.resize( n_nodes, 0 );
    }

    // E
    e.resize( n_nodes );
    for( auto & vec : e ){
      vec.resize( n_nodes );
      for( auto & vec2: vec ){
	vec2.fill( 0.0 );
      }
    }
	
  }

  void
  construct(
    RobotsGame const & game
  ){
    std::vector< Node > const nodes = get_all_nodes( game.board() );
    construct( game, nodes );
  }

  void
  construct(
    RobotsGame const & game,
    std::vector< Node > const & nodes
  ){
    //reset to all zeros
    reset( nodes.size() );

    for( uint i = 0; i < nodes.size(); ++i ){
      x[ i ] = GraphDecorator::calculate_node( nodes[i], game );

      for( uint j = i+1; j < nodes.size(); ++j ){
	if( GraphDecorator::edge_should_exist( nodes[i], nodes[j], game.board() ) ){
	  a[ i ][ j ] = a[ j ][ i ] = 1.0;
	  e[ i ][ j ] = GraphDecorator::calculate_edge( nodes[i], nodes[j] );
	  e[ j ][ i ] = GraphDecorator::calculate_edge( nodes[j], nodes[i] );
	}
      }
    }

    cached_nodes = nodes; //maybe std::move someday
  }

};

}
}
