#pragma once

#include <exception>
#include <string>

struct RobotsCoreException : public std::exception {

  std::string message_ = "RobotsCoreException";

  RobotsCoreException( std::string const & message ) :
    message_( message )
  {}

  char const * what() const throw() {
    return message_.c_str();
  }

};

//#define MY_ASSERT(x) if (!(x)) { throw(  STR(x), __PRETTY_FUNCTION__, __FILE__, __LINE__); abort(); }
//#define MY_ASSERT(x) if (!(x)) { printf("My custom assertion failed: (%s), function %s, file %s, line %d.\n", STR(x), __PRETTY_FUNCTION__, __FILE__, __LINE__); abort(); }
#define STR(x) #x
#define RC_ASSERT(x) if (!(x)) { throw RobotsCoreException( "'" + std::string(STR(x)) + "' failed on line " + std::to_string( __LINE__ ) + " in " + __FILE__ ); }

#define RC_ASSERT_EQUALS(x,y) if (!(x == y)) { throw RobotsCoreException( std::to_string(x) + " == " + std::to_string(y) + " failed on line " + std::to_string( __LINE__ ) + " in " + __FILE__ ); }

#define RC_ASSERT_DELTA( x , y, d ) if ( std::abs( x - y ) > d ) { throw RobotsCoreException( std::to_string(x) + " is within " + std::to_string(d) + " of " + std::to_string(y) + " failed on line " + std::to_string( __LINE__ ) + " in " + __FILE__ ); }

#ifndef RC_DEBUG
#define RC_DEBUG_ASSERT(condition) ((void)0)
#define RC_DEBUG_ASSERT_EQUALS(x,y) ((void)0)
#define RC_DEBUG_ASSERT_DELTA(x,y,d) ((void)0)
#else
#define RC_DEBUG_ASSERT(condition) RC_ASSERT( condition )
#define RC_DEBUG_ASSERT_EQUALS(x,y) RC_ASSERT_EQUALS(x,y)
#define RC_DEBUG_ASSERT_DELTA(x,y,d) RC_ASSERT_DELTA(x,y,d)
#endif
