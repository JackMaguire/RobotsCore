from robots_core import Board, RobotsGame, Occupant, Position
from robots_core.pocket import create_pocket

import curses
from curses import wrapper

def close( stdscr ):
    curses.nocbreak()
    stdscr.keypad(False)
    curses.echo()
    curses.endwin()

def draw_board( stdscr, board ):

    for i in range( 0, 45 ):
        for j in range( 0, 30 ):
            height = 30 - j
            c = board.cell( i, j )

            if (i+j)%2 == 0:
                x = curses.A_STANDOUT
            else:
                x = 0

            if c == Occupant.EMPTY:
                if (i+j)%2 == 0:
                    stdscr.addch( height, 2*i, ' ', curses.color_pair(246) )
                    stdscr.addch( height, 2*i+1, ' ', curses.color_pair(246) ) 
                else:
                    stdscr.addch( height, 2*i, ' ', curses.color_pair(250) )
                    stdscr.addch( height, 2*i+1, ' ', curses.color_pair(250) ) 
            elif c == Occupant.ROBOT:
                stdscr.addch( height, 2*i, 'R', curses.color_pair(40) )
                stdscr.addch( height, 2*i+1, 'b', curses.color_pair(40) )
            elif c == Occupant.FIRE:
                stdscr.addch( height, 2*i, 'E', curses.color_pair(161) )
                stdscr.addch( height, 2*i+1, 'x', curses.color_pair(161) )
            elif c == Occupant.HUMAN:
                stdscr.addch( height, 2*i, 'M', curses.color_pair(84) )
                stdscr.addch( height, 2*i+1, 'e', curses.color_pair(84) )
            
def draw_pocket2( stdscr, board ):

    pocket = create_pocket( board )
    distances = pocket.calculate_distances()

    for i in range( 0, 45 ):
        for j in range( 0, 30 ):
            height = 30 - j
            c = board.cell( i, j )

            if c == Occupant.EMPTY:
                if (i+j)%2 == 0:
                    stdscr.addstr( height, 2*i, str(distances[i][j]), curses.color_pair(246) )
                else:
                    stdscr.addstr( height, 2*i, str(distances[i][j]), curses.color_pair(250) )
            elif c == Occupant.ROBOT:
                stdscr.addch( height, 2*i, 'R', curses.color_pair(40) )
                stdscr.addch( height, 2*i+1, 'b', curses.color_pair(40) )
            elif c == Occupant.FIRE:
                stdscr.addch( height, 2*i, 'E', curses.color_pair(161) )
                stdscr.addch( height, 2*i+1, 'x', curses.color_pair(161) )
            elif c == Occupant.HUMAN:
                stdscr.addch( height, 2*i, 'M', curses.color_pair(84) )
                stdscr.addch( height, 2*i+1, 'e', curses.color_pair(84) )

def draw_pocket( stdscr, board ):

    pocket = create_pocket( board )
    distances = pocket.calculate_distances()

    for i in range( 0, 45 ):
        for j in range( 0, 30 ):
            height = 30 - j
            if distances[i][j] == 0:
                stdscr.addch( height, 2*i+1, 'P', curses.color_pair(99) )         


def draw_info( stdscr, game ):
    info = "N Safe Tele: {}   Current round: {}   Score: {}".format( game.n_safe_teleports_remaining(), game.round(), game.score() )
    stdscr.addstr( 31, 0, info, curses.color_pair(84) )

def draw_game( stdscr, game, show_pocket ):
    stdscr.clear()
    curses.start_color()
    #box1 = stdscr.subwin( 30, 45*2, 0, 0 )
    #box1.box()
    #draw_backgound( stdscr )
    draw_board( stdscr, game.board() )
    draw_info( stdscr, game )

    if show_pocket:
        #draw_pocket( stdscr, game.board() )
        draw_pocket2( stdscr, game.board() )
        pass
    
def main( stdscr ):
    # Clear screen
    stdscr.clear()
    stdscr.border(0)

    game = RobotsGame()
    show_pocket = False

    # This raises ZeroDivisionError when i == 10.
    draw_game( stdscr, game, show_pocket )

    while True:
        stdscr.refresh()
        k = stdscr.get_wch()

        if k in ( 'Q', 'q' ):
            game_over = game.move_human( -1, 1 )
            if( game_over ): return
            draw_game( stdscr, game, show_pocket )
        elif k in ( 'W', 'w' ):
            game_over = game.move_human( 0, 1 )
            if( game_over ): return
            draw_game( stdscr, game, show_pocket )
        elif k in ( 'E', 'e' ):
            game_over = game.move_human( 1, 1 )
            if( game_over ): return
            draw_game( stdscr, game, show_pocket )
        elif k in ( 'A', 'a' ):
            game_over = game.move_human( -1, 0 )
            if( game_over ): return
            draw_game( stdscr, game, show_pocket )
        elif k in ( 'S', 's' ):
            game_over = game.move_human( 0, 0 )
            if( game_over ): return
            draw_game( stdscr, game, show_pocket )
        elif k in ( 'D', 'd' ):
            game_over = game.move_human( 1, 0 )
            if( game_over ): return
            draw_game( stdscr, game, show_pocket )
        elif k in ( 'Z', 'z' ):
            game_over = game.move_human( -1, -1 )
            if( game_over ): return
            draw_game( stdscr, game, show_pocket )
        elif k in ( 'X', 'x' ):
            game_over = game.move_human( 0, -1 )
            if( game_over ): return
            draw_game( stdscr, game, show_pocket )
        elif k in ( 'C', 'c' ):
            game_over = game.move_human( 1, -1 )
            if( game_over ): return
            draw_game( stdscr, game, show_pocket )
        elif k in ( 'T', 't' ):
            game_over = game.teleport()
            if( game_over ): return
            draw_game( stdscr, game, show_pocket )
        elif k in ( ' ', ';' ):
            game_over = game.cascade()
            if( game_over ): return
            draw_game( stdscr, game, show_pocket )
        elif k == '1':
            show_pocket = not show_pocket
            draw_game( stdscr, game, show_pocket )
        elif k == '2':
            raise Exception( game.board().get_stringified_representation() )
        elif k in ( '~' ):
            return
        else:
            continue



if __name__ == '__main__':
    stdscr = curses.initscr()
    curses.noecho()
    curses.cbreak()
    stdscr.keypad(True)

    curses.start_color()
    curses.use_default_colors()
    for i in range(0, curses.COLORS):
        curses.init_pair(i + 1, curses.COLOR_BLACK, i)
    #curses.init_pair( 1, curses.COLOR_BLACK, curses.COLOR_GREEN )
    #curses.init_pair( 2, curses.COLOR_BLACK, curses.COLOR_RED )

    wrapper(main)

    close( stdscr )
    
