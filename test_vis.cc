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

int main(){
  RobotsGame const game;
  Board const & b = game.board();

  VisSettings vs;
  vs.label_elements = false;
  //vs.extra_nodes.push_back({ Position{ 1, 1 }, '7', "20,50,150" });
  //vs.edges.push_back({ Position{ 1, 1 }, Position{ 2, 2 } });
  //vs.edges.push_back({ Position{ 1, 1 }, Position{ 2, 1 } });

  add_all_edges_to_human( game, vs );

  std::cout << to_svg_string( b, vs ) << std::endl;
}
