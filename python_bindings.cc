#include <iostream>

#include <robots_core/game.hh>
#include <robots_core/forecasting.hh>
#include <robots_core/strategy/recursion.hh>
#include <robots_core/strategy/stall.hh>
#include <robots_core/graph/board_as_graph.hh>
#include <robots_core/graph/dense.hh>

#include <robots_core/asserts.hh>

#include <pybind11/pybind11.h>

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

    py::class_< Board > b (m, "Board");
    b.def( py::init<>() );
    b.def( py::init< std::string const & >() );
    b.def( "clear_board", &Board::clear_board );
    b.def( "init", &Board::init, py::arg("round") );
    
}
