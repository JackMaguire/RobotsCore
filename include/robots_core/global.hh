#pragma once

//#include <cstdlib> //rand()
//#include <algorithm>
#include <math.h> //sqrt
#include <type_traits>
#include <stdlib.h> //abs

namespace robots_core {

using GameOverBool = bool;

// signed char fails for some stupid reason
using sm_int = signed short int; //char; //-127 to 127

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
 OOB //out of bounds
};
constexpr unsigned char NOccupantTypes = 5;

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

  Position operator+( sm_int const val ) const {
    Position p;
    p.x = x + val;
    p.y = y + val;
    return p;
  }

  Position operator-( sm_int const val ) const {
    Position p;
    p.x = x - val;
    p.y = y - val;
    return p;
  }

  Position operator*( sm_int const val ) const {
    Position p;
    p.x = x * val;
    p.y = y * val;
    return p;
  }

  Position operator/( sm_int const val ) const {
    Position p;
    p.x = x / val;
    p.y = y / val;
    return p;
  }

  float distance( Position const & o ) const {
    Position const diff = (*this) - o;
    return sqrt( diff.x*diff.x + diff.y*diff.y );
  }

  float distance_sqaured( Position const & o ) const {
    Position const diff = (*this) - o;
    return diff.x*diff.x + diff.y*diff.y;
  }

  sm_int
  manhattan_distance( Position const & o ) const {
    return abs( x - o.x ) + abs( y - o.y );
  }

  //For python:
  static Position create( sm_int x_in, sm_int y_in ){
    return Position({x_in, y_in});
  }

};

} // namespace robots_core
