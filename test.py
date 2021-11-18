import robots_core
from robots_core.strategy import *
from robots_core.graph import *

g = robots_core.RobotsGame()

b = g.board()
print( b.cell( 0, 0 ) )
print( b.get_stringified_representation() )

graph = DenseGraph( g )
print( graph.copyX() )
