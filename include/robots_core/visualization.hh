#pragma once

#include <string>
#include <sstream>
#include <vector>
//#include <unordered_map>

#include <robots_core/board.hh>
#include <robots_core/asserts.hh>
#include <robots_core/graph/node.hh>

namespace robots_core {

template< int PixPerCell, int CircleOffset >
int convert_for_line( int const v ){
  return PixPerCell * v + CircleOffset;
}

struct VisNode {
  Position pos;
  char label = '?';
  std::string rgb = "20,50,150";

  std::string label_str() const {
    return std::to_string( label );
  }
};

struct MoveAnnotation {
  graph::SpecialCaseNode type;
  std::string rgb = "10,20,0";

  template< int PixPerCell >
  std::string
  string_for_tele( Position const hpos ) const {
    std::stringstream ss;

    constexpr int CircleOffset = PixPerCell/2;

    int const bx = hpos.x;
    int const by = hpos.y;
    int const i = bx;
    int const j = Board::HEIGHT - by - 1;
    //int const cx = i*PixPerCell + CircleOffset;
    //int const cy = j*PixPerCell + CircleOffset;
    int const tx = i*PixPerCell + CircleOffset/2 + (PixPerCell/4);
    int const ty = j*PixPerCell + (3*CircleOffset)/2 - (PixPerCell/4);

    ss << "<text x=\"" << tx << "\" y=\"" << ty << "\" class=\"tele\" fill=\"rgb(" << rgb << ")\">T</text>\n";

    return ss.str();

  }

  template< int PixPerCell >
  std::string
  string_for_stay( Position const hpos ) const {
    std::stringstream ss;

    constexpr int CircleOffset = PixPerCell/2;
    constexpr int CircleRadius = PixPerCell/4;

    int const bx = hpos.x;
    int const by = hpos.y;
    int const i = bx;
    int const j = Board::HEIGHT - by - 1;
    int const cx = i*PixPerCell + CircleOffset;
    int const cy = j*PixPerCell + CircleOffset;
    //int const tx = i*PixPerCell + CircleOffset/2;
    //int const ty = j*PixPerCell + (3*CircleOffset)/2;

    ss << "<circle stroke=\"black\" stroke-width=\"0\" fill=\"rgb(" << rgb << ")\" "
      "r=\"" << CircleRadius << "\" "
      "cx=\"" << cx << "\" "
      "cy=\"" << cy << "\" />\n";

    return ss.str();

  }

  template< int PixPerCell >
  std::string
  string_for_move(
    Position const hpos,
    int const dx,
    int const dy
  ) const {
    std::stringstream ss;

    Position p2 = hpos;
    p2.x += dx;
    p2.y += dy;

    constexpr int CircleOffset = PixPerCell/2;
    //constexpr int CircleRadius = PixPerCell/4;

    int const hpos_i = convert_for_line< PixPerCell, CircleOffset >( hpos.x );

    int const hpos_j = convert_for_line< PixPerCell, CircleOffset >( Board::HEIGHT-hpos.y-1 );

    int const p2_i = convert_for_line< PixPerCell, CircleOffset >( p2.x );

    int const p2_j = convert_for_line< PixPerCell, CircleOffset >( Board::HEIGHT-p2.y-1 );

    ss << "<line x1=\"" << hpos_i << "\" y1=\"" << hpos_j << "\" x2=\"" << p2_i << "\" y2=\"" << p2_j << "\" style=\"stroke:rgb(" << rgb << ");stroke-width:3\" />\n";

    return ss.str();

  }

  template< int PixPerCell >
  std::string
  annotation_string( Position const hpos ) const {

    if( type == graph::SpecialCaseNode::TELEPORT ){
      return string_for_tele< PixPerCell >( hpos );
    }

    int const dx = graph::dx_for_node( type );
    int const dy = graph::dx_for_node( type );

    if( dx == 0 and dy == 0 ){
      return string_for_stay< PixPerCell >( hpos );      
    }

    return string_for_move< PixPerCell >( hpos, dx, dy );
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

  std::vector< MoveAnnotation > moves;
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
      }// switch cell type
    }// HEIGHT
  }// WIDTH

  for( MoveAnnotation const & ann : settings.moves ){
    out << ann.annotation_string< PixPerCell >(
      board.human_position()
    );
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
    .tele { font: italic %%75TextHeight%%px sans-serif; }
</style>
<rect id="c1" fill="rgb(220,220,220)" width="%%PicWidth%%" height="%%PicHeight%%"/>)12345"; //";

  replaceFirstOccurrence( header, "%%PicWidth%%", std::to_string( PicWidth ) );
  replaceFirstOccurrence( header, "%%PicWidth%%", std::to_string( PicWidth ) );
  replaceFirstOccurrence( header, "%%PicHeight%%", std::to_string( PicHeight ) );
  replaceFirstOccurrence( header, "%%PicHeight%%", std::to_string( PicHeight ) );
  replaceFirstOccurrence( header, "%%TextHeight%%", std::to_string( CircleRadius ) );
  replaceFirstOccurrence( header, "%%75TextHeight%%", std::to_string( int(float(CircleRadius)*0.75) ) );

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
