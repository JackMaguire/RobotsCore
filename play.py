from robots_core import Board, RobotsGame, Occupant

import curses
from curses import wrapper

def close( stdscr ):
    curses.nocbreak()
    stdscr.keypad(False)
    curses.echo()
    curses.endwin()

def draw_backgound( stdscr ):
    #stdscr.bkgd(' ', curses.COLOR_BLUE )
    curses.init_pair(1, curses.COLOR_WHITE, curses.COLOR_BLUE)
    stdscr.bkgd(' ', curses.color_pair(1))
    dark = True
    for i in range( 0, 45 ):
        for j in range( 0, 30 ):
            height = 30 - j
            if dark:
                stdscr.addch( height, 2*i, 'X' )
                stdscr.addch( height, 2*i+1, 'X' )
                dark = False
            else:
                stdscr.addch( height, 2*i, 'X' )
                stdscr.addch( height, 2*i+1, 'X' )
                dark = True
        dark = not dark

def draw_foreground( stdscr, board ):
    for i in range( 0, 45 ):
        for j in range( 0, 30 ):
            height = 30 - j
            c = board.cell( i, j )
            if c == Occupant.EMPTY:
                if (i+j)%2 == 0:
                    stdscr.addch( height, 2*i, '-' )
                    stdscr.addch( height, 2*i+1, '-' )                    
            elif c == Occupant.ROBOT:
                stdscr.addch( height, 2*i, 'R' )
                stdscr.addch( height, 2*i+1, 'b' )
            elif c == Occupant.FIRE:
                stdscr.addch( height, 2*i, 'E' )
                stdscr.addch( height, 2*i+1, 'x' )
            elif c == Occupant.HUMAN:
                stdscr.addch( height, 2*i, 'M' )
                stdscr.addch( height, 2*i+1, 'e' )
            

def draw_board( stdscr, board ):
    stdscr.clear()
    curses.start_color()
    #box1 = stdscr.subwin( 30, 45*2, 0, 0 )
    #box1.box()
    #draw_backgound( stdscr )
    draw_foreground( stdscr, board )

    
    

def main( stdscr ):
    # Clear screen
    stdscr.clear()
    stdscr.border(0)

    game = RobotsGame()

    # This raises ZeroDivisionError when i == 10.
    draw_board( stdscr, game.board() )

    while True:
        stdscr.refresh()
        k = stdscr.get_wch()
        print(f'k: {k}')
        curses.flushinp()

        if k in ( 'Q', 'q' ):
            game_over = game.move_human( -1, 1 )
            if( game_over ): return
            draw_board( stdscr, game.board() )
        elif k in ( 'W', 'w' ):
            game_over = game.move_human( 0, 1 )
            if( game_over ): return
            draw_board( stdscr, game.board() )
        elif k in ( 'E', 'e' ):
            game_over = game.move_human( 1, 1 )
            if( game_over ): return
            draw_board( stdscr, game.board() )
        elif k in ( 'A', 'a' ):
            game_over = game.move_human( -1, 0 )
            if( game_over ): return
            draw_board( stdscr, game.board() )
        elif k in ( 'S', 's' ):
            game_over = game.move_human( 0, 0 )
            if( game_over ): return
            draw_board( stdscr, game.board() )
        elif k in ( 'D', 'd' ):
            game_over = game.move_human( 1, 0 )
            if( game_over ): return
            draw_board( stdscr, game.board() )
        elif k in ( 'Z', 'z' ):
            game_over = game.move_human( -1, -1 )
            if( game_over ): return
            draw_board( stdscr, game.board() )
        elif k in ( 'X', 'x' ):
            game_over = game.move_human( 0, -1 )
            if( game_over ): return
            draw_board( stdscr, game.board() )
        elif k in ( 'C', 'c' ):
            game_over = game.move_human( 1, -1 )
            if( game_over ): return
            draw_board( stdscr, game.board() )
        elif k in ( '~' ):
            return
        else:
            break



if __name__ == '__main__':
    stdscr = curses.initscr()
    curses.noecho()
    curses.cbreak()
    stdscr.keypad(True)

    wrapper(main)

    close( stdscr )
    
