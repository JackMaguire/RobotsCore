#pragma once

#include <robots_core/board.hh>

#include <array>

namespace robots_core{
namespace graph{

struct GraphDecorator {

constexpr static unsigned char F = Occupant::count + 3;
//Extra 3 elements are for possible moves
//+1: 1 if legal move, 0 otherwise
//+2: if legal move, ln( n_robots_killed ), -1 if n_robots_killed=0
//+3: number of safe teleports left

constexpr static unsigned char S = 4;
//0: ln( distance )-1
//1: ln( small_leg_dist )-1 // smaller cathetus of right triangle
//2: ln( long_leg_dist )-1  //  longer cathetus of right triangle
//3: ratio of small_leg_dist / long_leg_dist

};

enum SpecialCaseNodes {
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
  
  count //this should stay at the end
};

struct DenseGraph {



};

}
}
