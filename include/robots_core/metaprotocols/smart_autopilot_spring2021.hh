#pragma once

#include <robots_core/board.hh>
#include <robots_core/strategy/null_renderer.hh>
#include <robots_core/strategy/recursion.hh>
#include <robots_core/strategy/stall.hh>

#include <array>

namespace robots_core{

template< typename Renderer = NullRenderer >
void
loop_smart_autopilot(
  RobotsGame & game,
  Renderer & renderer
){
  while( true ){
    //Each call returns a GameOverBool

    if( skip_to_risky_w_recursion( ms ) ) break; //TODO

    ///////////////////
    // Deeper Recursion
    bool const recursive_solution_found = run_recursive_search< 6 >( game, 7, renderer );
    if( recursive_solution_found ) continue;

    //////////////////
    // Stall For Time
    if( stall_for_time( ms ) ) {
      if( run_recursive_search< 6 >( ms, 7 ) ) continue;
    }

    if( game_.n_safe_teleports_remaining() > 0 ) {
      bool const game_over = handle_move( 0, 0, true );
      if( game_over ) break;
    } else {
      bool const game_over = loop_autopilot( -1, ms, 1 );
      if( game_over ) break;
    }

  }
}

}
