#pragma once

#include <string>
#include <sstream>
#include <vector>

#include <robots_core/board.hh>
#include <robots_core/asserts.hh>

namespace robots_core {

//https://godbolt.org/z/dhPTbPon6
//https://stackoverflow.com/questions/5878775/how-to-find-and-replace-string/5878802
//TODO replace
/*void
replaceFirstOccurrence(
  std::string & s,
  std::string const & toReplace,
  std::string const & replaceWith)
{
  std::size_t pos = s.find( toReplace );
  s.replace( pos, toReplace.length(), replaceWith );
}

void
format(
  std::string & base,
  std::vector< std::string > const & tokens
){
  for( std::string const & s : tokens ){
    replaceFirstOccurrence( base, "%", s );
  }
}*/

template< typename Ostream >
std::string
to_svg( 
  Board const & board,
  Ostream & ostream
) {
  constexpr int PixPerCell = 10;
  constexpr int PicWidth = Board::WIDTH * PixPerCell;
  constexpr int PicHeight = Board::Height * PixPerCell;

  //std::string header = R"(<svg width="%" height="%")";
  //  format( 

  // Header
  ostream << "<svg width=\"" << PicWidth << "\" "
    "height=\"" << PicHeight << "\" >\n";

  // Background
  std::string const c1 = "dcdcdc";
  std::string const c2 = "c8c8c8";
  bool use_c1 = false;
  for( sm_int i = 0; i < WIDTH; ++i ){
    for( sm_int j = 0; j < HEIGHT; ++j ){
      use_c1 = !use_c1;
      if( use_c1 ) painter.setBrush(Wt::WBrush(c1));
      else         painter.setBrush(Wt::WBrush(c2));
      painter.drawRect( i*grid_size, j*grid_size, grid_size, grid_size );
    }
    use_c1 = !use_c1;
  }

  // Footer
  ostream << "<\svg>\n"
}

std::string
to_svg_string( Board const & board ) {
  std::stringstream ss;
  to_svg( board, ss );
  return ss.str();
}

} // namespace robots_core
