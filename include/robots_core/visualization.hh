#pragma once

#include <string>
#include <sstream>
#include <vector>
//#include <unordered_map>

#include <robots_core/board.hh>
#include <robots_core/asserts.hh>

namespace robots_core {

struct VisNode {
  Position pos;
  char label = '?';
  std::string rgb = "20,50,150";

  std::string label_str() const {
    return std::to_string( label );
  }
};

struct VisSettings {
  // If true, draw single-character labels on each element/
  // 'H' for human, 'R' for robot, etc
  bool label_elements = true;

  // Set nodes and colors for extra nodes
  std::vector< VisNode > extra_nodes;

  //                      | Position for node 1
  //                      |         | Position for node 2  
  //                      |         |
  std::vector< std::pair< Position, Position > > edges;
};

//https://godbolt.org/z/dhPTbPon6
//https://stackoverflow.com/questions/5878775/how-to-find-and-replace-string/5878802
//TODO replace
void
replaceFirstOccurrence(
  std::string & s,
  std::string const & toReplace,
  std::string const & replaceWith
) {
  std::size_t pos = s.find( toReplace );
  s.replace( pos, toReplace.length(), replaceWith );
}

/*void
format(
  std::string & base,
  std::vector< std::string > const & tokens
){
  for( std::string const & s : tokens ){
    replaceFirstOccurrence( base, "%", s );
  }
}*/

template< int PixPerCell, int CircleOffset, int CircleRadius, typename Ostream >
void
draw_background( Ostream & out ) {
  bool use_c1 = false;
  for( sm_int i = 0; i < Board::WIDTH; ++i ){
    for( sm_int j = 0; j < Board::HEIGHT; ++j ){
      use_c1 = !use_c1;
      if( not use_c1 ){
	out << "<rect fill=\"rgb(200,200,200)\" width=\"" << PixPerCell << "\" height=\"" << PixPerCell << "\" "
	  "x=\"" << i*PixPerCell << "\" y=\"" << j*PixPerCell << "\" />\n";
      }
    }
    use_c1 = !use_c1;
  }
}

template< int PixPerCell, int CircleOffset, int CircleRadius, typename Ostream >
void
draw_elements( 
  Board const & board,
  VisSettings const & settings,
  Ostream & out
) {
  for( sm_int i = 0; i < Board::WIDTH; ++i ){
    for( sm_int j = 0; j < Board::HEIGHT; ++j ){
      int const bx = i;
      int const by = Board::HEIGHT - j - 1;
      int const cx = i*PixPerCell + CircleOffset;
      int const cy = j*PixPerCell + CircleOffset;
      int const tx = i*PixPerCell + CircleOffset/2;
      int const ty = j*PixPerCell + (3*CircleOffset)/2;
      RC_ASSERT( Board::position_is_in_bounds( bx, by ) );
      switch( board.cell( bx, by ) ){
      case( Occupant::EMPTY ):
      case( Occupant::OOB ):
	break;
      case( Occupant::ROBOT ):
	out << "<circle stroke=\"black\" stroke-width=\"0\" fill=\"rgb(20,50,150)\" "
	  "r=\"" << CircleRadius << "\" "
	  "cx=\"" << cx << "\" "
	  "cy=\"" << cy << "\" />\n";
	if( settings.label_elements ){
	  out << "<text x=\"" << tx << "\" y=\"" << ty << "\" class=\"small\">R</text>\n";
	}
	break;
      case( Occupant::FIRE ):
	out << "<circle stroke=\"black\" stroke-width=\"0\" fill=\"red\" "
	  "r=\"" << CircleRadius << "\" "
	  "cx=\"" << cx << "\" "
	  "cy=\"" << cy << "\" />\n";
	if( settings.label_elements ){
	  out << "<text x=\"" << tx << "\" y=\"" << ty << "\" class=\"small\">X</text>\n";
	}
	break;
      case( Occupant::HUMAN ):
	out << "<circle stroke=\"black\" stroke-width=\"0\" fill=\"green\" "
	  "r=\"" << CircleRadius << "\" "
	  "cx=\"" << cx << "\" "
	  "cy=\"" << cy << "\" />\n";
	if( settings.label_elements ){
	  out << "<text x=\"" << tx << "\" y=\"" << ty << "\" class=\"small\">H</text>\n";
	}
	break;
      }
    }
  }
}

template< int PixPerCell, int CircleOffset, int CircleRadius, typename Ostream >
void
draw_extra_nodes( 
  VisSettings const & settings,
  Ostream & out
) {
  for( VisNode const & node : settings.extra_nodes ){
    int const bx = node.pos.x;
    int const by = node.pos.y;
    int const i = bx;
    int const j = Board::HEIGHT - by - 1;
    int const cx = i*PixPerCell + CircleOffset;
    int const cy = j*PixPerCell + CircleOffset;
    int const tx = i*PixPerCell + CircleOffset/2;
    int const ty = j*PixPerCell + (3*CircleOffset)/2;
    RC_ASSERT( Board::position_is_in_bounds( bx, by ) );

    out << "<circle stroke=\"black\" stroke-width=\"0\" fill=\"rgb(" + node.rgb + ")\" "
      "r=\"" << CircleRadius << "\" "
      "cx=\"" << cx << "\" "
      "cy=\"" << cy << "\" />\n";

    if( settings.label_elements ){
      out << "<text x=\"" << tx << "\" y=\"" << ty << "\" class=\"small\">" + node.label_str() + "</text>\n";
    }

  }
}

template< int PixPerCell, int CircleOffset >
int convert_for_line( int const v ){
  return PixPerCell * v + CircleOffset;
}

template< int PixPerCell, int CircleOffset, typename Ostream >
void
draw_edges( 
  VisSettings const & settings,
  Ostream & out
) {
  for( auto const & pos_pair : settings.edges ){
    Position const p1 = pos_pair.first;
    Position const p2 = pos_pair.second;

    int const p1_i = convert_for_line< PixPerCell, CircleOffset >( p1.x );

    int const p1_j = convert_for_line< PixPerCell, CircleOffset >( Board::HEIGHT-p1.y-1 );

    int const p2_i = convert_for_line< PixPerCell, CircleOffset >( p2.x );

    int const p2_j = convert_for_line< PixPerCell, CircleOffset >( Board::HEIGHT-p2.y-1 );

    out << "<line x1=\"" << p1_i << "\" y1=\"" << p1_j << "\" x2=\"" << p2_i << "\" y2=\"" << p2_j << "\" style=\"stroke:rgb(0,0,0);stroke-width:1\" />\n";
  }
}


template< typename Ostream >
void
to_svg( 
  Board const & board,
  VisSettings const & settings,
  Ostream & out
) {
  constexpr int PixPerCell = 20;
  constexpr int CircleOffset = PixPerCell/2;
  constexpr int CircleRadius = PixPerCell/2;
  constexpr int PicWidth = Board::WIDTH * PixPerCell;
  constexpr int PicHeight = Board::HEIGHT * PixPerCell;

  //std::string header = R"(<svg width="%" height="%")";
  //  format( 

  std::string header = R"12345(<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<svg xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" width="100%" height="100%" viewBox="0 0 %%PicWidth%% %%PicHeight%%">
<style>
    .small { font: italic %%TextHeight%%px sans-serif; fill: white; }
</style>
<rect id="c1" fill="rgb(220,220,220)" width="%%PicWidth%%" height="%%PicHeight%%"/>)12345"; //";

  replaceFirstOccurrence( header, "%%PicWidth%%", std::to_string( PicWidth ) );
  replaceFirstOccurrence( header, "%%PicWidth%%", std::to_string( PicWidth ) );
  replaceFirstOccurrence( header, "%%PicHeight%%", std::to_string( PicHeight ) );
  replaceFirstOccurrence( header, "%%PicHeight%%", std::to_string( PicHeight ) );
  replaceFirstOccurrence( header, "%%TextHeight%%", std::to_string( CircleRadius ) );

  // Header
  out << header << '\n';

  draw_background< PixPerCell, CircleOffset, CircleRadius >(out);
  draw_edges< PixPerCell, CircleOffset >( settings, out );
  draw_elements< PixPerCell, CircleOffset, CircleRadius >( board, settings, out );
  draw_extra_nodes< PixPerCell, CircleOffset, CircleRadius >( settings, out );

  // Footer
  out << "</svg>\n";
}

std::string
to_svg_string(
  Board const & board,
  VisSettings const & settings = VisSettings()
) {
  std::stringstream ss;
  to_svg( board, settings, ss );
  return ss.str();
}

} // namespace robots_core
