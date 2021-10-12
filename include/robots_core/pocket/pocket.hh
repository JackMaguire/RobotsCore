#pragma once

#include <robots_core/board.hh>

namespace robots_core {
namespace pocket {

enum class CardinalPost : unsigned char
  {
   UP,
   DOWN,
   LEFT,
   RIGHT
  };

enum class TerminalType : unsigned char
  {
   OOB,
   ROBOT
  };

enum class DiagonalQuadrant : unsigned char
  {
   UP_RIGHT,
   UP_LEFT,
   DOWN_LEFT,
   DOWN_RIGHT
  };


struct Post {
  Position pos;
  TerminalType type;
  unsigned char distance;
};

struct Pocket {

  Position human_position;

  std::array< Position, 4 > cardinal_posts;

  std::array< unsigned int, 4 > diagonal_offsets;
};

std::array< Post, 4 >
find_cardinal_posts( Board const & b ){
  std::array< Post, 4 > posts;

  Position const h = b.human_position();

  //UP
  {
    Position p = h;
    while( true ){
      p.y += 1;
      if( not Board::position_is_in_bounds( p ) ){
	p.y -= 1;
	posts[ CardinalPost::UP ].type = TerminalType::OOB;
	break;
      } else if( b.cell(p) == Occupant.ROBOT ){
	p.y -= 1;
	posts[ CardinalPost::UP ].type = TerminalType::ROBOT;
	break;
      }
    }
    posts[ CardinalPost::UP ].pos = p;
    posts[ CardinalPost::UP ].distance = p.y - h.y;
  } //up

  //DOWN
  {
    Position p = h;
    while( true ){
      p.y -= 1;
      if( not Board::position_is_in_bounds( p ) ){
	p.y += 1;
	posts[ CardinalPost::DOWN ].type = TerminalType::OOB;
	break;
      } else if( b.cell(p) == Occupant.ROBOT ){
	p.y += 1;
	posts[ CardinalPost::DOWN ].type = TerminalType::ROBOT;
	break;
      }
    }
    posts[ CardinalPost::DOWN ].pos = p;
    posts[ CardinalPost::DOWN ].distance = h.y - p.y;
  } //down

  //LEFT
  {
    Position p = h;
    while( true ){
      p.x -= 1;
      if( not Board::position_is_in_bounds( p ) ){
	p.x += 1;
	posts[ CardinalPost::LEFT ].type = TerminalType::OOB;
	break;
      } else if( b.cell(p) == Occupant.ROBOT ){
	p.x += 1;
	posts[ CardinalPost::LEFT ].type = TerminalType::ROBOT;
	break;
      }
    }
    posts[ CardinalPost::LEFT ].pos = p;
    posts[ CardinalPost::LEFT ].distance = h.x - p.x;
  } //left

  //RIGHT
  {
    Position p = h;
    while( true ){
      p.x += 1;
      if( not Board::position_is_in_bounds( p ) ){
	p.x -= 1;
	posts[ CardinalPost::RIGHT ].type = TerminalType::OOB;
	break;
      } else if( b.cell(p) == Occupant.ROBOT ){
	p.x -= 1;
	posts[ CardinalPost::RIGHT ].type = TerminalType::ROBOT;
	break;
      }
    }
    posts[ CardinalPost::RIGHT ].pos = p;
    posts[ CardinalPost::RIGHT ].distance = p.x - h.x;
  } //right

  return posts;
}

Pocket
create_pocket( Board const & b ){

  Pocket p;
  p.human_position = b.human_position();

  //Step 1: find 4 cardinal posts
  p.cardinal_posts = find_cardinal_posts( b );

  //Step 2: find diagonals
  

  return p;
}

} //namespace pocket
} //namespace robots_core
