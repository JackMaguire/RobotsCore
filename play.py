from robots_core import Board, RobotsGame

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
            if dark:
                stdscr.addch( j, 2*i, 'X' )
                stdscr.addch( j, 2*i+1, 'X' )
                dark = False
            else:
                stdscr.addch( j, 2*i, 'X' )
                stdscr.addch( j, 2*i+1, 'X' )
                dark = True
        dark = not dark

def draw_foreground( stdscr, board ):
    for p in board.robots():
        stdscr.addch( p.y, 2*p.x, 'R' )
        stdscr.addch( p.y, 2*p.x+1, 'R' )

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

    stdscr.refresh()
    stdscr.getkey()

if __name__ == '__main__':
    stdscr = curses.initscr()
    curses.noecho()
    curses.cbreak()
    stdscr.keypad(True)

    wrapper(main)

    close( stdscr )
    
