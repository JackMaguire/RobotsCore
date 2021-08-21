#pragma once

#include <array>
#include <vector>
#include <set>

#include <cstdlib> //rand()
#include <random>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <assert.h>
#include <thread>         // std::this_thread::sleep_for
#include <chrono>         // std::chrono::seconds
#include <math.h> //sqrt

//#define RC_USE_BOOST_SMALL_VEC //benchmarked - slightly faster with N = MAX_ROBOT / 10

#ifdef RC_USE_BOOST_SMALL_VEC
#include <boost/container/small_vector.hpp>
#endif

namespace robots_core {

using GameOverBool = bool;

// signed char fails for some stupid reason
using sm_int = signed short int; //char; //-127 to 127

constexpr sm_int WIDTH = 45;
constexpr sm_int HEIGHT = 30;

constexpr int nrobots_per_round( int round ){
  return round * 10;
}

constexpr int MAX_N_ROUNDS = 66;
constexpr int MAX_N_ROBOTS = nrobots_per_round( MAX_N_ROUNDS );
//MAX_N_ROUNDS * 10;

enum class Occupant : unsigned char
{
 EMPTY = 0,
 ROBOT,
 HUMAN,
 FIRE,
 OOB
};

enum class MoveResult : unsigned char
{
 CONTINUE = 0,
 YOU_LOSE = 1,
 YOU_WIN_ROUND = 2,
 YOU_WIN_GAME = 3
};


struct Position {
  
  bool operator==( Position const & o ) const {
    return x == o.x && y == o.y;
  }

  bool operator!=( Position const & o ) const {
    return ! ( *this == o );
  }

  sm_int x;
  sm_int y;

  Position operator+( Position const & o ) const {
    Position p;
    p.x = x + o.x;
    p.y = y + o.y;
    return p;
  }

  Position operator-( Position const & o ) const {
    Position p;
    p.x = x - o.x;
    p.y = y - o.y;
    return p;
  }

  float distance( Position const & o ) const {
    Position diff = (*this) - o;
    return sqrt( diff.x*diff.x + diff.y*diff.y );
  }

};

} // namespace robots_core
