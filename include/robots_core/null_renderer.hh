#pragma once

#include <robots_core/board.hh>

namespace robots_core{

struct NullRenderer {
  void operator()( RobotsGame const & ){};
};

}
