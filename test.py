import robots_core
from robots_core.strategy import *
from robots_core.graph import *

import numpy as np

g = robots_core.RobotsGame()

b = g.board()
print( b.cell( 0, 0 ) )
print( b.get_stringified_representation() )

graph = DenseGraph( g )
X = np.asarray( graph.x )
print( X.shape )

A = np.asarray( graph.a )
print( A.shape )

E = np.asarray( graph.e )
print( E.shape )

print( A )
