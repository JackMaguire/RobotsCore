import robots_core
from robots_core.strategy import *
from robots_core.graph import *

import numpy as np

from scipy import sparse
from spektral.data.utils import to_disjoint

def get_XAE():

    g = robots_core.RobotsGame( 30 )

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

    #print( A )
    return X, A, E

Xs = []
As = []
Es = []

for i in range( 0, 2 ):
    X, A, E = get_XAE()
    Xs.append( X )
    As.append( A )
    Es.append( E )

x, a, e, i = to_disjoint( Xs, [sparse.coo_matrix(a) for a in As], Es )

#np.set_printoptions(threshold=np.inf)

print( x.shape )
print( a.shape )
print( e.shape )
print( e )
#spy( a )
#for sample in a:
#    print( sample )
