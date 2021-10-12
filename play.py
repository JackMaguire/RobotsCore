from robots_core import Board, RobotsGame, Occupant

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
                #if (i+j)%2 == 0:
                stdscr.addch( height, 2*i, ' ', x )
                stdscr.addch( height, 2*i+1, ' ', x ) 
            elif c == Occupant.ROBOT:
                stdscr.addch( height, 2*i, 'R', x )
                stdscr.addch( height, 2*i+1, 'b', x )
            elif c == Occupant.FIRE:
                stdscr.addch( height, 2*i, 'E', x )
                stdscr.addch( height, 2*i+1, 'x', x )
            elif c == Occupant.HUMAN:
                stdscr.addch( height, 2*i, 'M', x | curses.A_BLINK )
                stdscr.addch( height, 2*i+1, 'e', x | curses.A_BLINK )
            

def draw_info( stdscr, game ):
    info = "N Safe Tele: {}   Current round: {}   Score: {}".format( game.n_safe_teleports_remaining(), game.round(), game.score() )
    stdscr.addstr( 31, 0, info )

def draw_game( stdscr, game ):
    stdscr.clear()
    curses.start_color()
    #box1 = stdscr.subwin( 30, 45*2, 0, 0 )
    #box1.box()
    #draw_backgound( stdscr )
    draw_board( stdscr, game.board() )
    draw_info( stdscr, game )

    
    

def main( stdscr ):
    # Clear screen
    stdscr.clear()
    stdscr.border(0)

    game = RobotsGame()

    # This raises ZeroDivisionError when i == 10.
    draw_game( stdscr, game )

    while True:
        stdscr.refresh()
        k = stdscr.get_wch()

        if k in ( 'Q', 'q' ):
            game_over = game.move_human( -1, 1 )
            if( game_over ): return
            draw_game( stdscr, game )
        elif k in ( 'W', 'w' ):
            game_over = game.move_human( 0, 1 )
            if( game_over ): return
            draw_game( stdscr, game )
        elif k in ( 'E', 'e' ):
            game_over = game.move_human( 1, 1 )
            if( game_over ): return
            draw_game( stdscr, game )
        elif k in ( 'A', 'a' ):
            game_over = game.move_human( -1, 0 )
            if( game_over ): return
            draw_game( stdscr, game )
        elif k in ( 'S', 's' ):
            game_over = game.move_human( 0, 0 )
            if( game_over ): return
            draw_game( stdscr, game )
        elif k in ( 'D', 'd' ):
            game_over = game.move_human( 1, 0 )
            if( game_over ): return
            draw_game( stdscr, game )
        elif k in ( 'Z', 'z' ):
            game_over = game.move_human( -1, -1 )
            if( game_over ): return
            draw_game( stdscr, game )
        elif k in ( 'X', 'x' ):
            game_over = game.move_human( 0, -1 )
            if( game_over ): return
            draw_game( stdscr, game )
        elif k in ( 'C', 'c' ):
            game_over = game.move_human( 1, -1 )
            if( game_over ): return
            draw_game( stdscr, game )
        elif k in ( 'T', 't' ):
            game_over = game.teleport()
            if( game_over ): return
            draw_game( stdscr, game )
        elif k in ( ' ', ';' ):
            game_over = game.cascade()
            if( game_over ): return
            draw_game( stdscr, game )
        elif k in ( '~' ):
            return
        else:
            continue



if __name__ == '__main__':
    stdscr = curses.initscr()
    curses.noecho()
    curses.cbreak()
    stdscr.keypad(True)

    wrapper(main)

    close( stdscr )
    
