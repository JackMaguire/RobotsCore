#pragma once

#include <iostream>
#include <robots_core/game.hh>

#include <vector>
#include <array>
#include <chrono>         // std::chrono::seconds

namespace robots_core{

struct NullRenderer {
  void render( RobotsGame const & ){};
};

template< typename Renderer = NullRenderer >
bool
stall_for_time(
  RobotsGame & game,
  Renderer & renderer
){
  bool any_solution_found = false;
  bool solution_found = true;

  while( solution_found ){
    solution_found = false;

    for( int dx = -1; dx <= 1; ++dx ){
      for( int dy = -1; dy <= 1; ++dy ){
        Board copy( game.board() );
        MoveResult const move_result = copy.move_human( dx, dy );

        if( move_result != MoveResult::CONTINUE ) continue;

        solution_found = copy.n_robots() == game_.board().n_robots();

	//GREEDY!
        if( solution_found ) {
          any_solution_found = true;
	  game.move_human( dx, dy );
          break;
        }
      }
      if( solution_found ) break;
    }

    renderer.render( game );
  }

  return any_solution_found;
}

bool
stall_for_time(
  RobotsGame & game
){
  Renderer r;
  return stall_for_time( game, r );
}


}
