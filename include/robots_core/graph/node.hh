#pragma once

#include <robots_core/game.hh>
#include <robots_core/forecasting.hh>
#include <robots_core/asserts.hh>

//#include <array>
#include <cmath>  //log
#include <vector>

namespace robots_core{
namespace graph{

enum class SpecialCaseNode : unsigned char {
  Q = 0,
  W,
  E,
  A,
  S,
  D,
  Z,
  X,
  C,
  LEFT_OOB,
  TOP_OOB,
  RIGHT_OOB,
  BOTTOM_OOB,
  
  count, //these should stay at the end
  NONE = count //these should stay at the end
};


struct Node {
  Position position;
  SpecialCaseNode special_case = SpecialCaseNode::NONE;

  int dx() const {
    switch( special_case ){
    case SpecialCaseNode::Q:
    case SpecialCaseNode::A:
    case SpecialCaseNode::Z:
      return -1;
    case SpecialCaseNode::W:
    case SpecialCaseNode::S:
    case SpecialCaseNode::X:
      return 0;
    case SpecialCaseNode::E:
    case SpecialCaseNode::D:
    case SpecialCaseNode::C:
      return 1;
    default:
      RC_DEBUG_ASSERT( false );
    }
  };

  int dy() const {
    switch( special_case ){
    case SpecialCaseNode::Z:
    case SpecialCaseNode::X:
    case SpecialCaseNode::C:
      return -1;
    case SpecialCaseNode::A:
    case SpecialCaseNode::S:
    case SpecialCaseNode::D:
      return 0;
    case SpecialCaseNode::Q:
    case SpecialCaseNode::W:
    case SpecialCaseNode::E:
      return 1;
    default:
      RC_DEBUG_ASSERT( false );
    }
  };
};

/*std::vector< Node >
determine_nodes_for_graph(
  RobotsGame const & game,
  unsigned int const max_nodes
){
  //TODO
}*/

}
}
