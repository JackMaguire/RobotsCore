#pragma once

#include <robots_core/board.hh>
#include <algorithm>
#include <array>
#include <iostream>
//#include <ranges>

namespace robots_core {
namespace pocket {

uint8_t
diff( uint8_t const a, uint8_t const b ){
  return std::max( a, b ) - std::min( a, b );
}

enum class CardinalPost : uint8_t
  {
   UP,
   DOWN,
   LEFT,
   RIGHT
  };

//for easy indexing
// |0 gets optimized out
uint8_t operator|(
  CardinalPost const a,
  uint8_t const b
){
  return (uint8_t)(a) | b;
}

enum class DiagonalQuadrant : uint8_t
  {
   UP_RIGHT,
   UP_LEFT,
   DOWN_LEFT,
   DOWN_RIGHT
  };

//for easy indexing
// |0 gets optimized out
uint8_t operator|(
  DiagonalQuadrant const a,
  uint8_t const b
){
  return (uint8_t)(a) | b;
}


struct Post {
  Position pos;
  uint8_t distance;

  std::string dist_str() const {
    return std::to_string( (unsigned int)distance );
  }
};

struct Pocket {

  Position center;
  std::array< Post, 4 > cardinal_posts;
  std::array< uint8_t, 4 > diagonal_offsets;

  Post & up() {
    return cardinal_posts[ CardinalPost::UP|0 ];
  }

  Post & down() {
    return cardinal_posts[ CardinalPost::DOWN|0 ];
  }

  Post & left() {
    return cardinal_posts[ CardinalPost::LEFT|0 ];
  }

  Post & right() {
    return cardinal_posts[ CardinalPost::RIGHT|0 ];
  }

  Post const & up() const {
    return cardinal_posts[ CardinalPost::UP|0 ];
  }

  Post const & down() const {
    return cardinal_posts[ CardinalPost::DOWN|0 ];
  }

  Post const & left() const {
    return cardinal_posts[ CardinalPost::LEFT|0 ];
  }

  Post const & right() const {
    return cardinal_posts[ CardinalPost::RIGHT|0 ];
  }

  uint8_t NW_offset() const {
    return diagonal_offsets[ DiagonalQuadrant::UP_LEFT|0 ];
  }

  uint8_t NE_offset() const {
    return diagonal_offsets[ DiagonalQuadrant::UP_RIGHT|0 ];
  }

  uint8_t SW_offset() const {
    return diagonal_offsets[ DiagonalQuadrant::DOWN_LEFT|0 ];
  }

  uint8_t SE_offset() const {
    return diagonal_offsets[ DiagonalQuadrant::DOWN_RIGHT|0 ];
  }

  bool contains_position( Position const & pos ) const;

  bool contains_position_within_window( Position const & pos, uint8_t const window ) const;
};

bool
Pocket::contains_position( Position const & pos ) const{
  if( pos.x > right().pos.x ) return false;
  if( pos.x < left().pos.x ) return false;
  if( pos.y > up().pos.y ) return false;
  if( pos.y < down().pos.y ) return false;

  if( pos.x == center.x ) return pos.y <=    up().pos.y
			    and  pos.y >=  down().pos.y;
  if( pos.y == center.y ) return pos.x <= right().pos.x
			    and  pos.x >=  left().pos.x;

  uint8_t const offset =
    diff( pos.x, center.x ) + diff( pos.y, center.y );


  if( pos.x < center.x and pos.y < center.y ){
    return offset <= SW_offset();
  }

  if( pos.x < center.x and pos.y > center.y ){
    return offset <= NW_offset();
  }

  if( pos.x > center.x and pos.y < center.y ){
    return offset <= SE_offset();
  }

  if( pos.x > center.x and pos.y > center.y ){
    return offset <= NE_offset();
  }

  return true;
}

bool
Pocket::contains_position_within_window(
  Position const & pos,
  uint8_t const window
) const{

  if( pos.x > right().pos.x + window ) return false;
  if( pos.x <  left().pos.x - window ) return false;
  if( pos.y >    up().pos.y + window ) return false;
  if( pos.y <  down().pos.y - window ) return false;

  if( pos.x >= center.x-window and pos.x <= center.x+window ){
    return pos.y <=    up().pos.y + window
      and  pos.y >=  down().pos.y - window;
  }
  if( pos.y >= center.y-window and pos.y <= center.y+window ){
    return pos.x <=  right().pos.x + window
      and  pos.x >=   left().pos.x - window;
  }

  uint8_t const offset =
    diff( pos.x, center.x ) + diff( pos.y, center.y );


  if( pos.x < center.x and pos.y < center.y ){
    return offset <= SW_offset() + window;
  }

  if( pos.x < center.x and pos.y > center.y ){
    return offset <= NW_offset() + window;
  }

  if( pos.x > center.x and pos.y < center.y ){
    return offset <= SE_offset() + window;
  }

  if( pos.x > center.x and pos.y > center.y ){
    return offset <= NE_offset() + window;
  }

  return true;
}


std::array< Post, 4 >
find_cardinal_posts( Board const & b ){
  std::array< Post, 4 > posts;

  Position const h = b.human_position();

  //UP
  {
    Post & post = posts[ (uint8_t) CardinalPost::UP ];

    Position p = h;
    while( true ){
      p.y += 1;
      if( not Board::position_is_in_bounds( p ) ){
	p.y -= 1;
	break;
      } else if( b.cell(p) == Occupant::ROBOT ){
	p.y -= 1;
	break;
      }
    }
    post.pos = p;
    post.distance = p.y - h.y;
  } //up

  //DOWN
  {
    Post & post = posts[ (uint8_t) CardinalPost::DOWN ];

    Position p = h;
    while( true ){
      p.y -= 1;
      if( not Board::position_is_in_bounds( p ) ){
	p.y += 1;
	break;
      } else if( b.cell(p) == Occupant::ROBOT ){
	p.y += 1;
	break;
      }
    }
    post.pos = p;
    post.distance = h.y - p.y;
  } //down

  //LEFT
  {
    Post & post = posts[ (uint8_t) CardinalPost::LEFT ];

    Position p = h;
    while( true ){
      p.x -= 1;
      if( not Board::position_is_in_bounds( p ) ){
	p.x += 1;
	break;
      } else if( b.cell(p) == Occupant::ROBOT ){
	p.x += 1;
	break;
      }
    }
    post.pos = p;
    post.distance = h.x - p.x;
  } //left

  //RIGHT
  {
    Post & post = posts[ (uint8_t) CardinalPost::RIGHT ];

    Position p = h;
    while( true ){
      p.x += 1;
      if( not Board::position_is_in_bounds( p ) ){
	p.x -= 1;
	break;
      } else if( b.cell(p) == Occupant::ROBOT ){
	p.x -= 1;
	break;
      }
    }
    post.pos = p;
    post.distance = p.x - h.x;
  } //right

  //std::cout << "post distances are " << posts[ CardinalPost::UP|0 ].dist_str() << ", " << posts[ CardinalPost::RIGHT|0 ].dist_str() << ", " << posts[ CardinalPost::DOWN|0 ].dist_str() << ", and " << posts[ CardinalPost::LEFT|0 ].dist_str() << std::endl;

  return posts;
}

template< typename QC, typename BC >
uint8_t
calc_diagonal(
  Board const & b,
  Pocket const & pocket,
  QC && quadrant_check,
  BC && box_check,
  uint8_t const max_dist
){
  // get_all_robots
  Board::PositionVec positions = b.robots();

  // erasing in rounds for the sake of sanity checking
  // remove robots in incorrect quandrants
  positions.erase(
    std::remove_if( positions.begin(), positions.end(), quadrant_check ),
    positions.end()
  );

  // remove robots that are farther than the posts
  positions.erase(
    std::remove_if( positions.begin(), positions.end(), box_check ),
    positions.end()
  );

  std::cout << positions.size() << " " << (int)max_dist << " " << std::endl;

  if( positions.empty() ){
    return max_dist;
  }
  
  //get closest Robot
  std::sort(
    positions.begin(),
    positions.end(),
    [&pocket]( Position const & a1, Position const & a2 ){
      return a1.distance( pocket.center ) < a2.distance( pocket.center );
    }    
  );
  Position const closest_robot = * positions.begin();

  uint8_t const offset = std::abs( closest_robot.x - pocket.center.x ) + std::abs( closest_robot.y - pocket.center.y ) - 1;

  return offset;
}

Pocket
create_pocket( Board const & b ){

  Pocket pocket;
  pocket.center = b.human_position();

  //Step 1: find 4 cardinal posts
  pocket.cardinal_posts = find_cardinal_posts( b );

  //Step 2: find diagonals
  pocket.diagonal_offsets[ DiagonalQuadrant::UP_LEFT|0 ] =
    calc_diagonal(
      b,
      pocket,
      [&pocket]( Position const & pos ){
	return pos.x >= pocket.center.x or pos.y <= pocket.center.y;
      },
      [&pocket]( Position const & pos ){
	return pos.y > pocket.up().pos.y or pos.x < pocket.left().pos.x;
      },
      pocket.up().distance + pocket.left().distance
    );

  pocket.diagonal_offsets[ DiagonalQuadrant::UP_RIGHT|0 ] =
    calc_diagonal(
      b,
      pocket,
      [&pocket]( Position const & pos ){
	return pos.x <= pocket.center.x or pos.y <= pocket.center.y;
      },
      [&pocket]( Position const & pos ){
	return pos.y > pocket.up().pos.y or pos.x > pocket.right().pos.x;
      },
      pocket.up().distance + pocket.right().distance
    );

  pocket.diagonal_offsets[ DiagonalQuadrant::DOWN_RIGHT|0 ] =
    calc_diagonal(
      b,
      pocket,
      [&pocket]( Position const & pos ){
	return pos.x <= pocket.center.x or pos.y >= pocket.center.y;
      },
      [&pocket]( Position const & pos ){
	return pos.y < pocket.down().pos.y or pos.x > pocket.right().pos.x;
      },
      pocket.down().distance + pocket.right().distance
    );

  pocket.diagonal_offsets[ DiagonalQuadrant::DOWN_LEFT|0 ] =
    calc_diagonal(
      b,
      pocket,
      [&pocket]( Position const & pos ){
	return pos.x >= pocket.center.x or pos.y >= pocket.center.y;
      },
      [&pocket]( Position const & pos ){
	return pos.y < pocket.down().pos.y or pos.x < pocket.left().pos.x;
      },
      pocket.down().distance + pocket.left().distance
    );

  //Resize Posts
  { //UP
    auto candidate = std::max( pocket.NW_offset(), pocket.NE_offset() );
    if( pocket.up().distance > candidate ){
      pocket.up().distance = candidate;
      pocket.up().pos.y = pocket.center.y + candidate;
    }
  }

  { //DOWN
    auto candidate = std::max( pocket.SW_offset(), pocket.SE_offset() );
    if( pocket.down().distance > candidate ){
      pocket.down().distance = candidate;
      pocket.down().pos.y = pocket.center.y - candidate;
    }
  }

  { //LEFT
    auto candidate = std::max( pocket.SW_offset(), pocket.NW_offset() );
    if( pocket.left().distance > candidate ){
      pocket.left().distance = candidate;
      pocket.left().pos.x = pocket.center.x - candidate;
    }
  }

  { //RIGHT
    auto candidate = std::max( pocket.SE_offset(), pocket.NE_offset() );
    if( pocket.right().distance > candidate ){
      pocket.right().distance = candidate;
      pocket.right().pos.x = pocket.center.x + candidate;
    }
  }

  return pocket;
}

} //namespace pocket
} //namespace robots_core
