#pragma once

#include <robots_core/board.hh>
#include <algorithm>
#include <array>
#include <iostream>

namespace robots_core {
namespace pocket {

unsigned char
diff( unsigned char const a, unsigned char const b ){
  return std::max( a, b ) - std::min( a, b );
}

enum class CardinalPost : unsigned char
  {
   UP,
   DOWN,
   LEFT,
   RIGHT
  };

//for easy indexing
// |0 gets optimized out
unsigned char operator|(
  CardinalPost const a,
  unsigned char const b
){
  return (unsigned char)(a) | b;
}

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

//for easy indexing
// |0 gets optimized out
unsigned char operator|(
  DiagonalQuadrant const a,
  unsigned char const b
){
  return (unsigned char)(a) | b;
}


struct Post {
  Position pos;
  TerminalType type;
  unsigned char distance;

  std::string dist_str() const {
    return std::to_string( (unsigned int)distance );
  }
};

struct Pocket {

  using BoardType = std::array< std::array< unsigned int, Board::HEIGHT >, Board::WIDTH >;


  Position center;
  std::array< Post, 4 > cardinal_posts;
  std::array< unsigned char, 4 > diagonal_offsets;

  BoardType calculate_distances() const;
};

Pocket::BoardType
Pocket::calculate_distances() const {
  using Card = CardinalPost;
  using Quad = DiagonalQuadrant;

  unsigned char const RightX = cardinal_posts[ Card::RIGHT|0 ].pos.x;
  unsigned char const LeftX = cardinal_posts[ Card::LEFT|0 ].pos.x;
  unsigned char const UpY = cardinal_posts[ Card::UP|0 ].pos.y;
  unsigned char const DownY = cardinal_posts[ Card::DOWN|0 ].pos.y;

  auto && min =
    []( unsigned char const a, unsigned char const b ){
      return std::min( a, b );
    };

  Pocket::BoardType distances;

  //zero out
  for( auto & a : distances ) a.fill( 0 );

  //Work your way around the octogon:
  
  //These are written in a way where the order can be switched

  //Top:
  for( unsigned char y = Board::HEIGHT-1; y > UpY; --y ){
    for( unsigned char x = 0; x < Board::WIDTH; ++x ){
      auto & val = distances[x][y];
      unsigned char const candidate_val = y - UpY;
      if( val == 0 ) val = candidate_val;
      else val = min( val, candidate_val );
    }
  }

  //Bottom:
  for( unsigned char y = 0; y < DownY; ++y ){
    for( unsigned char x = 0; x < Board::WIDTH; ++x ){
      auto & val = distances[x][y];
      unsigned char const candidate_val = DownY - y;
      if( val == 0 ) val = candidate_val;
      else val = min( val, candidate_val );
    }
  }

  //Left:
  for( unsigned char x = 0; x < LeftX; ++x ){
    for( unsigned char y = 0; y < Board::HEIGHT; ++y ){
      auto & val = distances[x][y];
      unsigned char const candidate_val = LeftX - x;
      if( val == 0 ) val = candidate_val;
      else val = min( val, candidate_val );      
    }
  }

  //Right:
  for( unsigned char x = Board::WIDTH-1; x > RightX; --x ){
    for( unsigned char y = 0; y < Board::HEIGHT; ++y ){
      auto & val = distances[x][y];
      unsigned char const candidate_val = x - RightX;
      if( val == 0 ) val = candidate_val;
      else val = min( val, candidate_val );      
    }
  }

  //Bottom-Left:
  {
    unsigned char const n_iter = center.x + center.y - diagonal_offsets[Quad::DOWN_LEFT|0];
    unsigned char const starting_x = center.x - diagonal_offsets[Quad::DOWN_LEFT|0];
    for( unsigned char i = 1; i <= n_iter; ++i ){
      Position p({ sm_int(starting_x - i + 1), sm_int(center.y-1) });
      while( true ){
	if( Board::position_is_in_bounds( p ) ){
	  distances[p.x][p.y] = i;
	}
	if( p.y == 0 or p.x == center.x-1 ) break;

	p.x += 1;
	p.y -= 1;
      }
    }//i
  }


  //Bottom-Right:
  {
    unsigned char const n_iter = (Board::WIDTH-1-center.x) + center.y - diagonal_offsets[Quad::DOWN_RIGHT|0];
    unsigned char const starting_x = center.x + diagonal_offsets[Quad::DOWN_RIGHT|0];
    for( unsigned char i = 1; i <= n_iter; ++i ){
      Position p({ sm_int(starting_x + i-1), sm_int(center.y-1) });
      while( true ){
	if( Board::position_is_in_bounds( p ) ){
	  distances[p.x][p.y] = i;
	}
	if( p.y == 0 or p.x == center.x+1 ) break;

	p.x -= 1;
	p.y -= 1;
      }
    }//i
  }

  //top-Right:
  {
    unsigned char const n_iter = (Board::WIDTH-1-center.x) + (Board::HEIGHT-1-center.y) - diagonal_offsets[Quad::UP_RIGHT|0];
    unsigned char const starting_x = center.x + diagonal_offsets[Quad::UP_RIGHT|0];
    for( unsigned char i = 1; i <= n_iter; ++i ){
      Position p({ sm_int(starting_x + i-1), sm_int(center.y+1) });
      while( true ){
	if( Board::position_is_in_bounds( p ) ){
	  distances[p.x][p.y] = i;
	}
	if( p.y == Board::HEIGHT-1 or p.x == center.x+1 ) break;

	p.x -= 1;
	p.y += 1;
      }
    }//i
  }

  //top-Left:
  {
    unsigned char const n_iter = center.x + (Board::HEIGHT-1-center.y) - diagonal_offsets[Quad::UP_LEFT|0];
    unsigned char const starting_x = center.x - diagonal_offsets[Quad::UP_LEFT|0];
    for( unsigned char i = 1; i <= n_iter; ++i ){
      Position p({ sm_int(starting_x-i+1), sm_int(center.y+1) });
      while( true ){
	if( Board::position_is_in_bounds( p ) ){
	  distances[p.x][p.y] = i;
	}
	if( p.y == Board::HEIGHT-1 or p.x == center.x-1 ) break;

	p.x += 1;
	p.y += 1;
      }
    }//i
  }

  return distances;
}


std::array< Post, 4 >
find_cardinal_posts( Board const & b ){
  std::array< Post, 4 > posts;

  Position const h = b.human_position();

  //UP
  {
    Post & post = posts[ (unsigned char) CardinalPost::UP ];

    Position p = h;
    while( true ){
      p.y += 1;
      if( not Board::position_is_in_bounds( p ) ){
	p.y -= 1;
	post.type = TerminalType::OOB;
	break;
      } else if( b.cell(p) == Occupant::ROBOT ){
	p.y -= 1;
	post.type = TerminalType::ROBOT;
	break;
      }
    }
    post.pos = p;
    post.distance = p.y - h.y;
  } //up

  //DOWN
  {
    Post & post = posts[ (unsigned char) CardinalPost::DOWN ];

    Position p = h;
    while( true ){
      p.y -= 1;
      if( not Board::position_is_in_bounds( p ) ){
	p.y += 1;
	post.type = TerminalType::OOB;
	break;
      } else if( b.cell(p) == Occupant::ROBOT ){
	p.y += 1;
	post.type = TerminalType::ROBOT;
	break;
      }
    }
    post.pos = p;
    post.distance = h.y - p.y;
  } //down

  //LEFT
  {
    Post & post = posts[ (unsigned char) CardinalPost::LEFT ];

    Position p = h;
    while( true ){
      p.x -= 1;
      if( not Board::position_is_in_bounds( p ) ){
	p.x += 1;
	post.type = TerminalType::OOB;
	break;
      } else if( b.cell(p) == Occupant::ROBOT ){
	p.x += 1;
	post.type = TerminalType::ROBOT;
	break;
      }
    }
    post.pos = p;
    post.distance = h.x - p.x;
  } //left

  //RIGHT
  {
    Post & post = posts[ (unsigned char) CardinalPost::RIGHT ];

    Position p = h;
    while( true ){
      p.x += 1;
      if( not Board::position_is_in_bounds( p ) ){
	p.x -= 1;
	post.type = TerminalType::OOB;
	break;
      } else if( b.cell(p) == Occupant::ROBOT ){
	p.x -= 1;
	post.type = TerminalType::ROBOT;
	break;
      }
    }
    post.pos = p;
    post.distance = p.x - h.x;
  } //right

  //std::cout << "post distances are " << posts[ CardinalPost::UP|0 ].dist_str() << ", " << posts[ CardinalPost::RIGHT|0 ].dist_str() << ", " << posts[ CardinalPost::DOWN|0 ].dist_str() << ", and " << posts[ CardinalPost::LEFT|0 ].dist_str() << std::endl;

  return posts;
}

unsigned char
calc_up_left_diagonal(
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
    ),
    positions.end()
  );

  // remove robots that are farther than the posts

  auto rm_iter =
    std::remove_if(
      positions.begin(), 
      positions.end(),
      [&pocket]( Position const & p ){
	return p.y >= pocket.cardinal_posts[ CardinalPost::UP|0 ].pos.y or p.x <= pocket.cardinal_posts[ CardinalPost::LEFT|0 ].pos.x;
      }
    );
  positions.erase( rm_iter, positions.end() );  

  if( positions.empty() ){
    return pocket.cardinal_posts[ CardinalPost::UP|0 ].distance + pocket.cardinal_posts[ CardinalPost::LEFT|0 ].distance;
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

  unsigned char const offset = std::abs( closest_robot.x - pocket.center.x ) + std::abs( closest_robot.y - pocket.center.y ) - 1;

  return offset;
}

template< typename QC, typename BC >
unsigned char
calc_diagonal(
  Board const & b,
  Pocket const & pocket,
  QC && quadrant_check,
  BC && box_check,
  unsigned char const max_dist
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

  unsigned char const offset = std::abs( closest_robot.x - pocket.center.x ) + std::abs( closest_robot.y - pocket.center.y ) - 1;

  return offset;
}


Pocket
create_pocket( Board const & b ){

  Pocket pocket;
  pocket.center = b.human_position();

  //Step 1: find 4 cardinal posts
  pocket.cardinal_posts = find_cardinal_posts( b );
  //For more readable code:
  auto const & posts = pocket.cardinal_posts;
  using CP = CardinalPost;

  //Step 2: find diagonals
  pocket.diagonal_offsets[ DiagonalQuadrant::UP_LEFT|0 ] =
    calc_diagonal(
      b,
      pocket,
      [&pocket,&posts]( Position const & pos ){
	return pos.x >= pocket.center.x or pos.y <= pocket.center.y;
      },
      [&pocket,&posts]( Position const & pos ){
	return pos.y > posts[ CP::UP|0 ].pos.y or pos.x < posts[ CP::LEFT|0 ].pos.x;
      },
      posts[ CP::UP|0 ].distance + posts[ CP::LEFT|0 ].distance
    );
  //assert( pocket.diagonal_offsets[ DiagonalQuadrant::UP_LEFT|0 ] == calc_up_left_diagonal( b, p ) );

  pocket.diagonal_offsets[ DiagonalQuadrant::UP_RIGHT|0 ] =
    calc_diagonal(
      b,
      pocket,
      [&pocket,&posts]( Position const & pos ){
	return pos.x <= pocket.center.x or pos.y <= pocket.center.y;
      },
      [&pocket,&posts]( Position const & pos ){
	return pos.y > posts[ CP::UP|0 ].pos.y or pos.x > posts[ CP::RIGHT|0 ].pos.x;
      },
      posts[ CP::UP|0 ].distance + posts[ CP::RIGHT|0 ].distance
    );

  pocket.diagonal_offsets[ DiagonalQuadrant::DOWN_RIGHT|0 ] =
    calc_diagonal(
      b,
      pocket,
      [&pocket,&posts]( Position const & pos ){
	return pos.x <= pocket.center.x or pos.y >= pocket.center.y;
      },
      [&pocket,&posts]( Position const & pos ){
	return pos.y < posts[ CP::DOWN|0 ].pos.y or pos.x > posts[ CP::RIGHT|0 ].pos.x;
      },
      posts[ CP::DOWN|0 ].distance + posts[ CP::RIGHT|0 ].distance
    );

  pocket.diagonal_offsets[ DiagonalQuadrant::DOWN_LEFT|0 ] =
    calc_diagonal(
      b,
      pocket,
      [&pocket,&posts]( Position const & pos ){
	return pos.x >= pocket.center.x or pos.y >= pocket.center.y;
      },
      [&pocket,&posts]( Position const & pos ){
	return pos.y < posts[ CP::DOWN|0 ].pos.y or pos.x < posts[ CP::LEFT|0 ].pos.x;
      },
      posts[ CP::DOWN|0 ].distance + posts[ CP::LEFT|0 ].distance
    );

  return pocket;
}

} //namespace pocket
} //namespace robots_core
