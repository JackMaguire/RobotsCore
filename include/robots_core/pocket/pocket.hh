#pragma once

#include <robots_core/board.hh>
#include <algorithm>

#include <iostream>

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

  Position center;

  std::array< Position, 4 > cardinal_posts;

  std::array< unsigned char, 4 > diagonal_offsets;

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

  std::cout << "post distances are " << posts[ CardinalPost::UP ].distance << ", " << posts[ CardinalPost::RIGHT ].distance << ", " << posts[ CardinalPost::DOWN ].distance << ", and " << posts[ CardinalPost::LEFT ].distance << std::endl;

  return posts;
}

unsigned char
calc_up_right_diagonal(
  Board const & b,
  Pocket const & pocket
){
  // get_all_robots
  Board::PositionVec positions = b.robots();

  // erasing in rounds for the sake of sanity checking
  // remove robots in incorrect quandrants
  positions.erase(
    std::remove_if(
      positions.begin(), 
      positions.end(),
      [&pocket]( Position const & p ){
	return p.x >= pocket.center.x or
	  p.y <= pocket.center.y;
      }
    )
  );
  std::cout << positions.size() << "robots in the NW" << std::endl;

  // remove robots that are farther than the posts
  positions.erase(
    std::remove_if(
      positions.begin(), 
      positions.end(),
      [&pocket]( Position const & p ){
	return p.y >= pocket.cardinal_posts[ CardinalPost::UP ].pos.y or p.x <= pocket.cardinal_posts[ CardinalPost::LEFT ].pos.x;
      }
    )
  );
  std::cout << positions.size() << "robots in the NW box" << std::endl;
  

  if( positions.empty() ){
    return pocket.cardinal_posts[ CardinalPost::UP ].distance + pocket.cardinal_posts[ CardinalPost::LEFT ].distance;
  }
  
  //get closest Robot
  std::sort(
    positions.begin(),
    positions.end(),
    [&pocket]( Position const & a, Position const & b ){
      return a.distance( pocket.center ) < b.distance( pocket.center );
    }    
  );
  Position const closest_robot = * positions.begin();

  std::cout << "Closest robot in the NW is " << closest_robot.distance( pocket.center ) << " spaces away from the human" << std::endl;

  unsigned char const offset = std::abs( closest_robot.x - pocket.center.x ) + std::abs( closest_robot.y - pocket.center.y );

  std::cout << "NW offset is " << offset << std::endl;

  return offset;
}

Pocket
create_pocket( Board const & b ){

  Pocket p;
  p.center = b.human_position();

  //Step 1: find 4 cardinal posts
  p.cardinal_posts = find_cardinal_posts( b );

  //Step 2: find diagonals
  

  return p;
}

} //namespace pocket
} //namespace robots_core
