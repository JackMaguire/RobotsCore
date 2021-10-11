#include <iostream>

#include <robots_core/game.hh>
#include <robots_core/forecasting.hh>
#include <robots_core/strategy/recursion.hh>
#include <robots_core/strategy/stall.hh>
#include <robots_core/graph/board_as_graph.hh>
#include <robots_core/graph/dense.hh>

#include <robots_core/asserts.hh>

#include <pybind11/pybind11.h>
#include <pybind11/operators.h>

namespace py = pybind11;
using namespace robots_core;

//g++ python_bindings.cc -o robots_core$(python3-config --extension-suffix) -O3 -Wall -Wextra -Iinclude -Iextern/pybind11/include -std=c++17 -fPIC $(python3 -m pybind11 --includes) -shared

PYBIND11_MODULE(robots_core, m) {
    m.doc() = "Implementation of the Robots game"; // optional module docstring
    //m.def("add", &add, "A function which adds two numbers");

    py::enum_< Occupant >( m, "Occupant" )
      .value( "EMPTY", Occupant::EMPTY )
      .value( "ROBOT", Occupant::ROBOT )
      .value( "HUMAN", Occupant::HUMAN )
      .value( "FIRE", Occupant::FIRE )
      .value( "OOB", Occupant::OOB )
      ;

    py::enum_< MoveResult >( m, "MoveResult" )
      .value( "CONTINUE", MoveResult::CONTINUE )
      .value( "YOU_LOSE", MoveResult::YOU_LOSE )
      .value( "YOU_WIN_ROUND", MoveResult::YOU_WIN_ROUND )
      .value( "YOU_WIN_GAME", MoveResult::YOU_WIN_GAME )
      ;

    py::class_< Position > p( m, "Position" );
    p.def( py::init<>() );
    p.def( py::self + py::self );
    p.def( py::self - py::self );
    p.def( py::self == py::self );
    p.def( py::self != py::self );
    p.def( "distance", &Position::distance );

    py::class_< Board > b( m, "Board" );
    b.def( py::init<>() );
    b.def( py::init< std::string const & >() );

    //No-verloads
    b.def( "clear_board", &Board::clear_board );
    b.def( "init", &Board::init, py::arg("round") );
    b.def( "n_robots", &Board::n_robots );
    b.def( "human_position", &Board::human_position );
    b.def( "robots", &Board::robots );

    b.def( "cell_is_safe_for_teleport", &Board::cell_is_safe_for_teleport );
    b.def( "teleport", &Board::teleport );
    b.def( "move_robots_1_step", &Board::move_robots_1_step );
    b.def( "move_human", &Board::move_human );
    b.def( "move_is_safe", &Board::move_is_safe );

    b.def( "get_stringified_representation", &Board::get_stringified_representation );
    b.def( "load_from_stringified_representation", &Board::load_from_stringified_representation );
    
    //Overloads
    b.def( "cell", static_cast< Occupant &( Board::* )( Position const & )>( &Board::cell ) );
    b.def( "cell", static_cast< Occupant &( Board::* )( int, int )>( &Board::cell ) );
    b.def( "move_is_cascade_safe", py::overload_cast< sm_int, sm_int >( &Board::move_is_cascade_safe, py::const_ ) );


    py::class_< RobotsGame > g( m, "RobotsGame" );
    g.def( py::init<>() );
    g.def( "load_from_stringified_representation", 
      &RobotsGame::load_from_stringified_representation );
    g.def( "reset", &RobotsGame::reset );
    g.def( "cascade", &RobotsGame::renderless_cascade );
    g.def( "move_human", &RobotsGame::move_human );
    g.def( "teleport", &RobotsGame::teleport );
    g.def( "board", &RobotsGame::board );
    g.def( "n_safe_teleports_remaining", &RobotsGame::n_safe_teleports_remaining );
    g.def( "round", &RobotsGame::round );
    g.def( "latest_result", &RobotsGame::latest_result );

    py::class_< ForecastResults > fr( m, "ForecastResults" );
    fr.def_readonly( "legal", &ForecastResults::legal );
    fr.def_readonly( "cascade_safe", &ForecastResults::cascade_safe );
    fr.def_readonly( "robots_killed", &ForecastResults::robots_killed );

    //Strategy
    py::module m_strat = m.def_submodule( "strategy" );

    py::class_< Move > move( m_strat, "Move" );
    move.def_readwrite( "dx", &Move::dx );
    move.def_readwrite( "dy", &Move::dy );
    move.def( "nullop", &Move::nullop );
    move.def( "set_nullop", &Move::set_nullop );

    m_strat.def( "run_recursive_seach_3", &run_recursive_seach<3> );
    m_strat.def( "run_recursive_seach_4", &run_recursive_seach<4> );
    m_strat.def( "run_recursive_seach_5", &run_recursive_seach<5> );
    m_strat.def( "run_recursive_seach_6", &run_recursive_seach<6> );
    m_strat.def( "run_recursive_seach_7", &run_recursive_seach<7> );

    //m_strat.def( "stall_for_time", static_cast< bool( RobotsGame & game ) >( &stall_for_time ) )
    m_strat.def( "stall_for_time", &stall_for_time );
}
