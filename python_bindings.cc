#include <iostream>

#include <robots_core/game.hh>
#include <robots_core/forecasting.hh>
#include <robots_core/strategy/recursion.hh>
#include <robots_core/strategy/stall.hh>
#include <robots_core/graph/board_as_graph.hh>
#include <robots_core/graph/dense.hh>
#include <robots_core/pocket/pocket.hh>
#include <robots_core/visualization.hh>

#include <robots_core/asserts.hh>

#include <pybind11/pybind11.h>
#include <pybind11/operators.h>
#include <pybind11/stl.h>

namespace py = pybind11;
using namespace robots_core;

//g++ python_bindings.cc -o robots_core$(python3-config --extension-suffix) -O3 -Wall -Wextra -Iinclude -Iextern/pybind11/include -std=c++17 -fPIC $(python3 -m pybind11 --includes) -shared

bool
run_recursive_seach_dyn( RobotsGame & game, int const n_tele_desired, int const depth ){
  switch( depth ){
  case( 1 ): return run_recursive_seach< 1 >( game, n_tele_desired );
  case( 2 ): return run_recursive_seach< 2 >( game, n_tele_desired );
  case( 3 ): return run_recursive_seach< 3 >( game, n_tele_desired );
  case( 4 ): return run_recursive_seach< 4 >( game, n_tele_desired );
  case( 5 ): return run_recursive_seach< 5 >( game, n_tele_desired );
  case( 6 ): return run_recursive_seach< 6 >( game, n_tele_desired );
  case( 7 ): return run_recursive_seach< 7 >( game, n_tele_desired );
  case( 8 ): return run_recursive_seach< 8 >( game, n_tele_desired );
  case( 9 ): return run_recursive_seach< 9 >( game, n_tele_desired );
  default: return false;
  }
}

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
    //p.def( py::init<>() );
    p.def( py::self + py::self );
    p.def( py::self - py::self );
    p.def( py::self == py::self );
    p.def( py::self != py::self );
    p.def( "distance", &Position::distance );
    p.def( "create", &Position::create );
    p.def_readonly( "x", &Position::x );
    p.def_readonly( "y", &Position::y );

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
    g.def( py::init< int >() );
    g.def( py::init< int, int >() );
    g.def( "load_from_stringified_representation", 
      &RobotsGame::load_from_stringified_representation );
    g.def( "reset", &RobotsGame::reset );
    g.def( "cascade", &RobotsGame::renderless_cascade );
    g.def( "move_human", &RobotsGame::move_human );
    g.def( "teleport", &RobotsGame::teleport );
    g.def( "board", &RobotsGame::board );
    g.def( "n_safe_teleports_remaining", &RobotsGame::n_safe_teleports_remaining );
    g.def( "round", &RobotsGame::round );
    g.def( "score", &RobotsGame::score );
    g.def( "latest_result", &RobotsGame::latest_result );
    g.def( "game_is_over", &RobotsGame::game_is_over );

    py::class_< ForecastResults > fr( m, "ForecastResults" );
    fr.def_readonly( "legal", &ForecastResults::legal );
    fr.def_readonly( "cascade_safe", &ForecastResults::cascade_safe );
    fr.def_readonly( "robots_killed", &ForecastResults::robots_killed );

    m.def( "forecast_move", &forecast_move );
    m.def( "forecast_all_moves", &forecast_all_moves );

    //Strategy
    py::module m_strat = m.def_submodule( "strategy" );

    py::class_< Move > move( m_strat, "Move" );
    move.def_readwrite( "dx", &Move::dx );
    move.def_readwrite( "dy", &Move::dy );
    move.def( "nullop", &Move::nullop );
    move.def( "set_nullop", &Move::set_nullop );

    m_strat.def( "run_recursive_seach_1", &run_recursive_seach<1> );
    m_strat.def( "run_recursive_seach_2", &run_recursive_seach<2> );
    m_strat.def( "run_recursive_seach_3", &run_recursive_seach<3> );
    m_strat.def( "run_recursive_seach_4", &run_recursive_seach<4> );
    m_strat.def( "run_recursive_seach_5", &run_recursive_seach<5> );
    m_strat.def( "run_recursive_seach_6", &run_recursive_seach<6> );
    m_strat.def( "run_recursive_seach_7", &run_recursive_seach<7> );
    m_strat.def( "run_recursive_seach", &run_recursive_seach_dyn );

    //m_strat.def( "stall_for_time", static_cast< bool( RobotsGame & game ) >( &stall_for_time ) )
    m_strat.def( "stall_for_time", &stall_for_time );


    //Graph
    py::module m_graph = m.def_submodule( "graph" );

    py::enum_< graph::SpecialCaseNode >( m_graph, "SpecialCaseNode" )
      .value( "Q", graph::SpecialCaseNode::Q )
      .value( "W", graph::SpecialCaseNode::W )
      .value( "E", graph::SpecialCaseNode::E )

      .value( "A", graph::SpecialCaseNode::A )
      .value( "S", graph::SpecialCaseNode::S )
      .value( "D", graph::SpecialCaseNode::D )

      .value( "Z", graph::SpecialCaseNode::Z )
      .value( "X", graph::SpecialCaseNode::X )
      .value( "C", graph::SpecialCaseNode::C )

      .value( "TELEPORT", graph::SpecialCaseNode::TELEPORT )

      .value( "LEFT_OOB", graph::SpecialCaseNode::LEFT_OOB )
      .value( "TOP_OOB", graph::SpecialCaseNode::TOP_OOB )
      .value( "RIGHT_OOB", graph::SpecialCaseNode::RIGHT_OOB )
      .value( "BOTTOM_OOB", graph::SpecialCaseNode::BOTTOM_OOB )

      .value( "NONE", graph::SpecialCaseNode::NONE )
      .value( "count", graph::SpecialCaseNode::count );

    py::class_< graph::Node > node( m_graph, "Node" );
    node.def_readonly( "occupant", &graph::Node::occupant );
    node.def_readonly( "position", &graph::Node::position );
    node.def_readonly( "special_case", &graph::Node::special_case );
    node.def_readonly( "orientation", &graph::Node::orientation );

    node.def( "dx", &graph::Node::dx );
    node.def( "dy", &graph::Node::dy );

    using robots_core::graph::DenseGraph;
    py::class_< DenseGraph > dg( m_graph, "DenseGraph" );
    dg.def( py::init<>() );
    dg.def( py::init< RobotsGame const & >() );
    dg.def_readonly( "x", &DenseGraph::x );
    dg.def_readonly( "a", &DenseGraph::a );
    dg.def_readonly( "e", &DenseGraph::e );
    dg.def_readonly( "cached_nodes", &DenseGraph::cached_nodes );

    //Pocket
    using namespace robots_core::pocket;
    py::module m_pocket = m.def_submodule( "pocket" );

    py::enum_< CardinalPost >( m_pocket, "CardinalPost" )
      .value( "UP", CardinalPost::UP )
      .value( "DOWN", CardinalPost::DOWN )
      .value( "LEFT", CardinalPost::LEFT )
      .value( "RIGHT", CardinalPost::RIGHT )
      ;

    py::enum_< DiagonalQuadrant >(m_pocket, "DiagonalQuadrant")
      .value( "UP_RIGHT", DiagonalQuadrant::UP_RIGHT )
      .value( "UP_LEFT", DiagonalQuadrant::UP_LEFT )
      .value( "DOWN_LEFT", DiagonalQuadrant::DOWN_LEFT )
      .value( "DOWN_RIGHT", DiagonalQuadrant::DOWN_RIGHT )
      ;

    py::class_< Post > post( m_pocket, "Post" );
    post.def_readonly( "pos", &Post::pos );
    post.def_readonly( "distance", &Post::distance );

    py::class_< Pocket > pocket( m_pocket, "Pocket" );
    pocket.def_readonly( "center", &Pocket::center );
    pocket.def_readonly( "cardinal_posts", &Pocket::cardinal_posts );
    pocket.def_readonly( "diagonal_offsets", &Pocket::diagonal_offsets );
    pocket.def( "contains_position", &Pocket::contains_position );
    pocket.def( "contains_position_within_window", &Pocket::contains_position_within_window );
    
    m_pocket.def( "find_cardinal_posts", &find_cardinal_posts );
    m_pocket.def( "create_pocket", &create_pocket );

    
    //Visualization
    py::module m_vis = m.def_submodule( "visualization" );
    m_vis.def( "to_svg_string", &robots_core::to_svg_string );


    py::class_< VisNode > vis_node( m_vis, "VisNode" );
    vis_node.def_readwrite( "pos", &VisNode::pos );
    vis_node.def_readwrite( "label", &VisNode::label );
    vis_node.def_readwrite( "rgb", &VisNode::rgb );

    
    py::class_< MoveAnnotation > move_ann( m_vis, "MoveAnnotation" );
    move_ann.def( py::init<>() );
    move_ann.def_readwrite( "type", &MoveAnnotation::type );
    move_ann.def_readwrite( "rgb", &MoveAnnotation::rgb );


    py::class_< VisSettings > vis_settings( m_vis, "VisSettings" );
    vis_settings.def( py::init<>() );
    vis_settings.def_readwrite( "label_elements", &VisSettings::label_elements );
    vis_settings.def_readwrite( "extra_nodes", &VisSettings::extra_nodes );
    vis_settings.def_readwrite( "edges", &VisSettings::edges );
    vis_settings.def_readwrite( "moves", &VisSettings::moves );
    vis_settings.def( "append_move", []( VisSettings & vs, MoveAnnotation & ma ){
      vs.moves.push_back( ma );
    } );


#ifndef RC_EXPAND_PYMODULE
}
#endif //RC_EXPAND_PYMODULE
