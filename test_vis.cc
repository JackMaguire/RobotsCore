#include <iostream>

#include <robots_core/game.hh>
#include <robots_core/visualization.hh>
#include <robots_core/graph/dense.hh>

//Example: g++ test_vis.cc -o test_vis -O3 -Wall -Wextra -Iinclude -std=c++17

using namespace robots_core;

int
get_human_node_ind(
  graph::DenseGraph const & dg
){
  for( unsigned int i = 0; i < dg.cached_nodes.size(); ++i ){
    //if( dg.cached_nodes[i].occupant == Occupant::HUMAN ){
    if( dg.cached_nodes[i].special_case == graph::SpecialCaseNode::S ){
      return i;
    }
  }
  RC_ASSERT( false );
  return 0;
}

void
add_all_edges_to_human(
  RobotsGame const & game,
  VisSettings & vs
){
  graph::DenseGraph dg( game );
  int const human_i = get_human_node_ind( dg );
  Position const human_pos = dg.cached_nodes[ human_i ].position;

  auto const & human_A_slice = dg.a[ human_i ];
  for( unsigned int i = 0; i < human_A_slice.size(); ++i ){
    if( human_A_slice[i] > 0 ){
      //std::cout << "<!-- Edge with: " << i << " -->\n";
      vs.edges.push_back({ human_pos, dg.cached_nodes[ i ].position });
    }
  }
}

void
add_all_robot_robot_edges(
  RobotsGame const & game,
  VisSettings & vs
){
  graph::DenseGraph dg( game );

  for( unsigned int i = 0; i < dg.cached_nodes.size()-1; ++i ){
    if( dg.cached_nodes[i].occupant != Occupant::ROBOT ) continue;
    for( unsigned int j = i+1; j < dg.cached_nodes.size(); ++j ){
      if( dg.cached_nodes[j].occupant != Occupant::ROBOT ) continue;

      if( dg.a[i][j] > 0 ){
	vs.edges.push_back({
	    dg.cached_nodes[ i ].position,
	      dg.cached_nodes[ j ].position
	      });
      }
	
    }
  }
  
}

void
add_all_edges(
  RobotsGame const & game,
  VisSettings & vs
){
  graph::DenseGraph dg( game );

  for( unsigned int i = 0; i < dg.cached_nodes.size()-1; ++i ){
    for( unsigned int j = i+1; j < dg.cached_nodes.size(); ++j ){

      if( dg.a[i][j] > 0 ){
	vs.edges.push_back({
	    dg.cached_nodes[ i ].position,
	      dg.cached_nodes[ j ].position
	      });
      }
	
    }
  }
  
}

void
add_extra_nodes(
  RobotsGame const & game,
  VisSettings & vs
){
  graph::DenseGraph dg( game );

  for( unsigned int i = 0; i < dg.cached_nodes.size(); ++i ){
    if( dg.cached_nodes[i].occupant == Occupant::EMPTY ){
      vs.extra_nodes.push_back({ dg.cached_nodes[i].position, ' ', "100,150,150" });  
    }
  }
}

int main(){
  RobotsGame const game( 10 );
  Board const & b = game.board();

  VisSettings vs;
  //vs.label_elements = false;

  //add_extra_nodes( game, vs );

  //add_all_edges( game, vs );
  //add_all_edges_to_human( game, vs );
  add_all_robot_robot_edges( game, vs );

  vs.moves.push_back({ graph::SpecialCaseNode::S, "3,219,252"});
  vs.moves.push_back({ graph::SpecialCaseNode::TELEPORT, "182,3,252"});
  vs.moves.push_back({ graph::SpecialCaseNode::Q, "3,20,25"});

  std::cout << to_svg_string( b, vs ) << std::endl;
}
