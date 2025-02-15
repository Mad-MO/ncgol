
// File:    ncgol.c
// Author:  Martin Ochs
// License: MIT
// Brief:   A very basic python implementation for Conway's "Game of Life"
//          using ncurses for the GUI. The simulation can be controlled by
//          the arrow keys and the buttons for different initial states.
//
// Ncurses: https://tldp.org/HOWTO/NCURSES-Programming-HOWTO/
// Unicode: https://www.compart.com/en/unicode/block/U+2580
//          https://www.compart.com/en/unicode/block/U+2800

// TODO: Commandline options
// TODO: Improve the end detection (Large grid with gliders)
// TODO: Add more patterns

#include <curses.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <locale.h>
#include <ctype.h>
#include <time.h>
#include "grid.h"



// Define SW name and Version
#define SW_NAME "ncgol"
#define SW_VERS "v0.4"
#define AUTHOR  "by domo"

static uint8_t  speed;
static uint16_t grid_width;
static uint16_t grid_height;

WINDOW *w_grid_box;
WINDOW *w_grid;
WINDOW *w_status_box;
WINDOW *w_status;

static PatternType pattern;

typedef enum
{
    StageTypeStartup,   // Show startup screen
    StageTypeStartwait, // Timeout for startup window
    StageTypeInit,      // Initialize grid
    StageTypeShowInfo,  // Show info for current pattern
    StageTypeRunning,   // Running simulation
    StageTypeEnd        // End of simulation has been reached
} StageType;
static StageType stage;

typedef enum
{
    // Styles which use two chars per cell
    StyleTypeBlock1,
    StyleTypeBlock2,
    StyleTypeHash,

    // Styles which show two cells in one char
    StyleTypeDoubleBlock,
    StyleTypeDoubleDot,

    // Braile style with 8 dots per char
    StyleTypeBraille,
    // ----------------
    StyleTypeMax
} StyleType;
static StyleType style;

typedef enum
{
    COLOR_PAIR_STANDARD = 1,
    COLOR_PAIR_LABEL,
    COLOR_PAIR_VALUE,
    COLOR_PAIR_LIVE_CELL,
    COLOR_PAIR_TITLE,
    COLOR_PAIR_PATTERN_INFO
} ColorPairType;
static ColorPairType color_pair;

typedef enum
{
    MODE_NEXT, // When current pattern ends, switch to next pattern
    MODE_LOOP, // When current pattern ends, restart current pattern
    MODE_STOP, // When current pattern ends, stop the simulation
    // ----------------
    MODE_MAX
} ModeType;
static ModeType mode;



// Function to initialize the User Interface
void init_tui(void)
{
    // Set locale
    setlocale(LC_ALL, "");

    // Initialize ncurses
    initscr();   // Determine terminal type
    cbreak();    // Disable buffering
    noecho();    // Disable echo to the screen
    curs_set(0); // Hide the cursor
    clear();     // Clear the screen

    // Prepare coloring
    start_color();
    init_pair(COLOR_PAIR_STANDARD,     COLOR_WHITE,   COLOR_BLACK);   // Standard
    init_pair(COLOR_PAIR_LABEL,        COLOR_GREEN,   COLOR_BLACK);   // Label
    init_pair(COLOR_PAIR_VALUE,        COLOR_YELLOW,  COLOR_BLACK);   // Value
    init_pair(COLOR_PAIR_LIVE_CELL,    COLOR_WHITE,   COLOR_BLACK);   // Live cell
    init_pair(COLOR_PAIR_TITLE,        COLOR_CYAN,    COLOR_BLACK);   // Title
    init_pair(COLOR_PAIR_PATTERN_INFO, COLOR_WHITE,   COLOR_MAGENTA); // Pattern info
    ESCDELAY = 1; // Set the delay for escape sequences

    // Create status window
    w_status_box = newwin(3, COLS, LINES-3, 0);
    box(w_status_box, 0, 0); // Draw a box around the screen
    wattron(w_status_box, COLOR_PAIR(COLOR_PAIR_TITLE));
    mvwaddstr(w_status_box, 0, 3, " Status ");
    wattroff(w_status_box, COLOR_PAIR(0));
    wrefresh(w_status_box);
    w_status = newwin(1, COLS-2, LINES-2, 1);
    keypad(w_status, TRUE); // Enable special keys

    // Create grid window
    w_grid_box = newwin(LINES-3, COLS, 0, 0);
    box(w_grid_box, 0, 0); // Draw a box around the screen
    wattron(w_grid_box, COLOR_PAIR(COLOR_PAIR_TITLE));
    mvwaddstr(w_grid_box, 0, 3, " Game of Life ");
    wattroff(w_grid_box, COLOR_PAIR(COLOR_PAIR_TITLE));
    wrefresh(w_grid_box);
    w_grid = newwin(LINES-5, COLS-2, 1, 1);
    nodelay(w_grid, TRUE); // Non-blocking input
    keypad(w_grid, TRUE); // Enable special keys

    // Init
    if     (style == StyleTypeBraille)
    {
        grid_width  = getmaxx(w_grid) * 2;
        grid_height = getmaxy(w_grid) * 4;
    }
    else if(style >= StyleTypeDoubleBlock)
    {
        grid_width  = getmaxx(w_grid);
        grid_height = getmaxy(w_grid)*2;
    }
    else
    {
        grid_width  = getmaxx(w_grid) / 2;
        grid_height = getmaxy(w_grid);
    }
    if(grid_width  > GRID_WIDTH_MAX)  grid_width  = GRID_WIDTH_MAX;
    if(grid_height > GRID_HEIGHT_MAX) grid_height = GRID_HEIGHT_MAX;
    set_grid_size(grid_width, grid_height);
}



// Function to draw a string in a rounded frame for the pattern name
static void draw_str_in_frame(const char * str)
{
    uint16_t len=strlen(str);
    uint16_t x = (getmaxx(w_grid) - len) / 2 - 2;
    uint16_t y = (getmaxy(w_grid)) / 4;

    wattron(w_grid, A_BOLD | COLOR_PAIR(COLOR_PAIR_PATTERN_INFO));
    for(uint16_t i=0; i<len+4; i++)
    {
        mvwaddch(w_grid, y+0, x+i, ' ');
        mvwaddch(w_grid, y+1, x+i, ' ');
        mvwaddch(w_grid, y+2, x+i, ' ');
    }
    mvwaddstr(w_grid, y+1, x+2, str);
    wattroff(w_grid, A_BOLD | COLOR_PAIR(COLOR_PAIR_PATTERN_INFO));
}



// Function to draw the grid on the canvas
static void draw_grid(void)
{
    uint16_t x, y;
    char str[16];
    grid_t * grid = get_grid();

    // Draw grid to canvas
    wattron(w_grid, A_BOLD | COLOR_PAIR(COLOR_PAIR_LIVE_CELL));
    for(x=0; x<grid_width; x++)
    {
        for(y=0; y<grid_height; y++)
        {
            if((style == StyleTypeDoubleBlock) || (style == StyleTypeDoubleDot))
            {
                // Two dots per character
                if(grid[x][y] && grid[x][y + 1])
                {
                    if     (style == StyleTypeDoubleBlock)
                        mvwaddstr(w_grid, y/2, x, "\u2588");
                    else          // StyleTypeDoubleDot
                        mvwaddch(w_grid, y/2, x, ':');
                }
                else if(grid[x][y])
                {
                    if     (style == StyleTypeDoubleBlock)
                        mvwaddstr(w_grid, y/2, x, "\u2580");
                    else          // StyleTypeDoubleDot
                        mvwaddch(w_grid, y/2, x, '\'');
                }
                else if(grid[x][y + 1])
                {
                    if     (style == StyleTypeDoubleBlock)
                        mvwaddstr(w_grid, y/2, x, "\u2584");
                    else          // StyleTypeDoubleDot
                        mvwaddch(w_grid, y/2, x, '.');
                }
                else
                {
                    mvwaddch(w_grid, y/2, x, ' ');
                }
                y++;
            }
            else if(style == StyleTypeBraille)
            {
                // The braille pattern allows the usage of 8 dots per character
                uint16_t braille = 0;
                wchar_t braille_char;
                char braille_str[4] = {0};

                // Convert braille pattern to unicode character
                if(grid[x+0][y+0]) {braille |= 0x01;}
                if(grid[x+0][y+1]) {braille |= 0x02;}
                if(grid[x+0][y+2]) {braille |= 0x04;}
                if(grid[x+0][y+3]) {braille |= 0x40;}
                if(grid[x+1][y+0]) {braille |= 0x08;}
                if(grid[x+1][y+1]) {braille |= 0x10;}
                if(grid[x+1][y+2]) {braille |= 0x20;}
                if(grid[x+1][y+3]) {braille |= 0x80;}
                braille |= 0x2800;
                braille_char = braille;
                wcstombs(braille_str, &braille_char, 4);

                mvwaddstr(w_grid, y/4, x/2, braille_str);

                y+=3;
                if(y >= (grid_height-1))
                    x+=1;
            }
            else
            {
                // Two characters represent one cell
                // Using background color with an empy space works not very well in ncurses,
                // because the background color is only dimmed and not bright.
                // A unicode full block uses the foreground color and works better.
                if(grid[x][y])
                {
                    if     (style == StyleTypeBlock1)
                        mvwaddstr(w_grid, y, x*2, "\u2588\u2588"); // Two full blocks -> Looks best on a linux terminal which leaves no horizontal space between the blocks
                    else if(style == StyleTypeBlock2)
                        mvwaddstr(w_grid, y, x*2, "\u2588\u258a"); // Full block and 3/4 block -> Looks best on a mac terminal which leaves a little horizontal space between the blocks
                    else          // StyleTypeHash
                        mvwaddstr(w_grid, y, x*2, "# ");           // #  -> Looks best on a terminal which has problems with unicode characters
                }
                else
                {
                    mvwaddstr(w_grid, y, x*2, "  ");
                }
            }
        }
    }
    wattroff(w_grid, A_BOLD);

    // Handle startup screen
    if((stage == StageTypeStartup) || (stage == StageTypeStartwait))
    {
        wmove(w_grid, 0, 0);
        waddstr(w_grid, " \n");
        wattron(w_grid, A_BOLD | A_REVERSE);
        waddstr(w_grid, " A ncurses based Simulation for Conways \"Game of Life\" \n");
        wattroff(w_grid, A_BOLD | A_REVERSE);
        waddstr(w_grid, " \n");
        wprintw(w_grid, " Maximum grid size:    %ux%u\n", GRID_WIDTH_MAX, GRID_HEIGHT_MAX);
        waddstr(w_grid, " \n");
        waddstr(w_grid, " Basic command keys:\n");
        waddstr(w_grid, "   \'q\'                 End program\n");
        waddstr(w_grid, "   \'0\'...\'9\'           Set speed directly\n");
        waddstr(w_grid, "   \'Up\' and \'Down\'     Adjust speed\n");
        waddstr(w_grid, "   \'Left\' and \'Right\'  Change pattern\n");
        waddstr(w_grid, "   \'Space\'             Restart current pattern\n");
        waddstr(w_grid, "   \'s\'                 Change style\n");
        waddstr(w_grid, "   \'m\'                 Change mode\n");
        waddstr(w_grid, "   \'h\'                 This Help\n");
        waddstr(w_grid, " \n");
        waddstr(w_grid, " Pattern keys:\n");
        waddstr(w_grid, "   \'r\'                 Random\n");
        waddstr(w_grid, "   \'b\'                 Blinker\n");
        waddstr(w_grid, "   \'g\'                 Glider\n");
        waddstr(w_grid, "   \'l\'                 Glider gun\n");
        waddstr(w_grid, "   \'p\'                 Pentomino\n");
        waddstr(w_grid, "   \'d\'                 Diehard\n");
        waddstr(w_grid, "   \'a\'                 Acorn\n");
    }

    // Handle pattern info
    if(stage == StageTypeShowInfo)
    {
        if     (pattern == PatternTypeRandom)
            draw_str_in_frame("Random");
        else if(pattern == PatternTypeBlinker)
            draw_str_in_frame("Blinker");
        else if(pattern == PatternTypeGlider)
            draw_str_in_frame("Glider");
        else if(pattern == PatternTypeGliderGun)
            draw_str_in_frame("Glider gun");
        else if(pattern == PatternTypePentomino)
            draw_str_in_frame("Pentomino");
        else if(pattern == PatternTypeDiehard)
            draw_str_in_frame("Diehard");
        else if(pattern == PatternTypeAcorn)
            draw_str_in_frame("Acorn");
    }

    // Handle end message
    if(stage == StageTypeEnd)
    {
        draw_str_in_frame("End of simulation");
    }

    // Handle status line
    {
        // Full line ==> "Grid:2500x1000 Cycles:123456 Cells:1500000 Speed:10 Pattern:Glider gun Style:DoubleBlock   ncgol v0.x by domo"
        char str_label[20];
        char str_value[20];
        uint16_t pos   = 0;
        uint16_t width = getmaxx(w_status);
        wmove(w_status, 0, 0);

        // Grid
        strcpy(str_label, " Grid:");
        sprintf(str_value, "%ux%u", grid_width, grid_height);
        if((getcurx(w_status)+strlen(str_label)+strlen(str_value)) < width)
        {
            wattron(w_status, COLOR_PAIR(COLOR_PAIR_LABEL));
            waddstr(w_status, str_label);
            wattron(w_status, COLOR_PAIR(COLOR_PAIR_VALUE));
            waddstr(w_status, str_value);
        }

        // Cycles
        strcpy(str_label, " Cycles:");
        sprintf(str_value, "%3u", get_cycle_counter());
        if((getcurx(w_status)+strlen(str_label)+strlen(str_value)) < width)
        {
            wattron(w_status, COLOR_PAIR(COLOR_PAIR_LABEL));
            waddstr(w_status, str_label);
            wattron(w_status, COLOR_PAIR(COLOR_PAIR_VALUE));
            waddstr(w_status, str_value);
        }

        // Cells
        strcpy(str_label, " Cells:");
        sprintf(str_value, "%3u", get_cells_alive());
        if((getcurx(w_status)+strlen(str_label)+strlen(str_value)) < width)
        {
            wattron(w_status, COLOR_PAIR(COLOR_PAIR_LABEL));
            waddstr(w_status, str_label);
            wattron(w_status, COLOR_PAIR(COLOR_PAIR_VALUE));
            waddstr(w_status, str_value);
        }

        // Speed
        strcpy(str_label, " Speed:");
        sprintf(str_value, "%u", speed);
        if((getcurx(w_status)+strlen(str_label)+strlen(str_value)) < width)
        {
            wattron(w_status, COLOR_PAIR(COLOR_PAIR_LABEL));
            waddstr(w_status, str_label);
            wattron(w_status, COLOR_PAIR(COLOR_PAIR_VALUE));
            waddstr(w_status, str_value);
        }

        // Pattern
        strcpy(str_label, " Pattern:");
        if     (pattern == PatternTypeRandom)
            strcpy(str_value, "Random");
        else if(pattern == PatternTypeBlinker)
            strcpy(str_value, "Blinker");
        else if(pattern == PatternTypeGlider)
            strcpy(str_value, "Glider");
        else if(pattern == PatternTypeGliderGun)
            strcpy(str_value, "Glider gun");
        else if(pattern == PatternTypePentomino)
            strcpy(str_value, "Pentomino");
        else if(pattern == PatternTypeDiehard)
            strcpy(str_value, "Diehard");
        else if(pattern == PatternTypeAcorn)
            strcpy(str_value, "Acorn");
        else
            strcpy(str_value, "?");
        if((getcurx(w_status)+strlen(str_label)+strlen(str_value)) < width)
        {
            wattron(w_status, COLOR_PAIR(COLOR_PAIR_LABEL));
            waddstr(w_status, str_label);
            wattron(w_status, COLOR_PAIR(COLOR_PAIR_VALUE));
            waddstr(w_status, str_value);
        }

        // Style
        strcpy(str_label, " Style:");
        if     (style == StyleTypeBlock1)
            strcpy(str_value, "Block1");
        else if(style == StyleTypeBlock2)
            strcpy(str_value, "Block2");
        else if(style == StyleTypeHash)
            strcpy(str_value, "Hash");
        else if(style == StyleTypeDoubleBlock)
            strcpy(str_value, "DoubleBlock");
        else if(style == StyleTypeDoubleDot)
            strcpy(str_value, "DoubleDot");
        else if(style == StyleTypeBraille)
            strcpy(str_value, "Braille");
        else
            strcpy(str_value, "?");
        if((getcurx(w_status)+strlen(str_label)+strlen(str_value)) < width)
        {
            wattron(w_status, COLOR_PAIR(COLOR_PAIR_LABEL));
            waddstr(w_status, str_label);
            wattron(w_status, COLOR_PAIR(COLOR_PAIR_VALUE));
            waddstr(w_status, str_value);
        }

        // Mode
        strcpy(str_label, " Mode:");
        if     (mode == MODE_NEXT)
            strcpy(str_value, "Next");
        else if(mode == MODE_LOOP)
            strcpy(str_value, "Loop");
        else if(mode == MODE_STOP)
            strcpy(str_value, "Stop");
        else
            strcpy(str_value, "?");
        if((getcurx(w_status)+strlen(str_label)+strlen(str_value)) < width)
        {
            wattron(w_status, COLOR_PAIR(COLOR_PAIR_LABEL));
            waddstr(w_status, str_label);
            wattron(w_status, COLOR_PAIR(COLOR_PAIR_VALUE));
            waddstr(w_status, str_value);
        }

        // Remember position and clear rest of line
        waddstr(w_status, "   ");
        pos = getcurx(w_status);
        for(uint16_t i=pos; i<width; i++)
            waddch(w_status, ' ');

        // Author
        if((width-strlen(AUTHOR)-1) > pos)
        {
            wattron(w_status, COLOR_PAIR(COLOR_PAIR_STANDARD));
            mvwaddstr(w_status, 0, width-strlen(AUTHOR)-1, AUTHOR);
        }

        // SW Version
        if((width-strlen(AUTHOR)-1-strlen(SW_VERS)-1) > pos)
        {
            wattron(w_status, COLOR_PAIR(COLOR_PAIR_VALUE));
            mvwaddstr(w_status, 0, width-strlen(AUTHOR)-1-strlen(SW_VERS)-1, SW_VERS);
        }

        // SW Name
        if((width-strlen(AUTHOR)-1-strlen(SW_VERS)-1-strlen(SW_NAME)-1) > pos)
        {
            wattron(w_status, COLOR_PAIR(COLOR_PAIR_STANDARD));
            mvwaddstr(w_status, 0, width-strlen(AUTHOR)-1-strlen(SW_VERS)-1-strlen(SW_NAME)-1, SW_NAME);
        }
    }

    wattroff(w_status, COLOR_PAIR(COLOR_PAIR_VALUE));
    wrefresh(w_status);
}



// Function to handle input events
void handle_inputs(void)
{
    int key = wgetch(w_grid);
    if(tolower(key) == 'q')
    {
        endwin();
        exit(0);
    }
    if(key==27) // ESC
    {
        if     (stage == StageTypeStartwait)
        {
            stage = StageTypeInit;
        }
        else if(stage == StageTypeShowInfo)
        {
            stage = StageTypeRunning;
        }
    }
    else if(key == KEY_UP)
    {
        if(speed < 10) speed++;
    }
    else if(key == KEY_DOWN)
    {
        if(speed > 1) speed--;
    }
    else if(key == KEY_RIGHT)
    {
        pattern++;
        pattern %= PatternTypeCycleMax;
        stage = StageTypeInit;
    }
    else if(key == KEY_LEFT)
    {
        if(pattern == 0) pattern = PatternTypeCycleMax - 1;
        else             pattern--;
        stage = StageTypeInit;
    }
    else if(tolower(key) == 's')
    {
        style++;
        style %= StyleTypeMax;
        init_tui();
    }
    else if(tolower(key) == 'm')
    {
        mode++;
        mode %= MODE_MAX;
    }
    else if(key==KEY_RESIZE)
    {
        init_tui();
    }
    else if((key>='1') && (key<='9'))
    {
        speed = key - '0';
    }
    else if(key == '0')
    {
        speed = 10;
    }
    else if(key == ' ')
    {
        stage   = StageTypeInit;
    }
    else if(tolower(key) == 'r')
    {
        pattern = PatternTypeRandom;
        stage   = StageTypeInit;
    }
    else if(tolower(key) == 'b')
    {
        pattern = PatternTypeBlinker;
        stage   = StageTypeInit;
    }
    else if(tolower(key) == 'g')
    {
        pattern = PatternTypeGlider;
        stage   = StageTypeInit;
    }
    else if(tolower(key) == 'l')
    {
        pattern = PatternTypeGliderGun;
        stage   = StageTypeInit;
    }
    else if(tolower(key) == 'p')
    {
        pattern = PatternTypePentomino;
        stage   = StageTypeInit;
    }
    else if(tolower(key) == 'd')
    {
        pattern = PatternTypeDiehard;
        stage   = StageTypeInit;
    }
    else if(tolower(key) == 'a')
    {
        pattern = PatternTypeAcorn;
        stage   = StageTypeInit;
    }
    else if(tolower(key) == 'h')
    {
        stage   = StageTypeStartup;
    }
    else
    {
        // Do nothing
    }
}



// Function to handle one life cycle of the simulation
int main(void)
{
    // Initialize variables
    pattern = PatternTypeRandom;
    speed   = 3;
    mode    = MODE_NEXT;
    #if  (defined __APPLE__)
        style = StyleTypeBlock2;
    #elif(defined __linux__)
        style = StyleTypeBlock1;
    #else
        style = StyleTypeHash;
    #endif

    // Initialize ncurses and grid
    init_tui();
    wrefresh(w_grid);

    // Init
    static uint16_t last_ticks;
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    last_ticks = ts.tv_nsec / 1000000;

    // Loop until back key is pressed
    while(1)
    {
        static uint16_t timer;
               uint16_t ticks;

        // Handle passed time
        clock_gettime(CLOCK_REALTIME, &ts);
        ticks = ts.tv_nsec / 1000000; // ticks = 0...999
        timer += (ticks - last_ticks + 1000) % 1000;
        last_ticks = ticks;

        // Handle input
        handle_inputs();

        // Handle speed
        if     (speed == 1) usleep(500000);
        else if(speed == 2) usleep(100000);
        else if(speed == 3) usleep( 50000);
        else if(speed == 4) usleep( 10000);
        else if(speed == 5) usleep(  5000);
        else if(speed == 6) usleep(  1000);
        else if(speed == 7) usleep(   500);
        else if(speed == 8) usleep(   100);
        else if(speed == 9) usleep(    50);
        else                usleep(     0);

        // Handle different patterns and update grid
        if     (stage == StageTypeStartup)
        {
            init_grid(PatternTypeClear);
            stage = StageTypeStartwait;
            timer = 0;
        }
        else if(stage == StageTypeStartwait)
        {
            if(timer >= 20000)
            {
                stage = StageTypeInit;
                timer = 0;
            }
        }
        else if(stage == StageTypeInit)
        {
            init_grid(pattern);
            stage = StageTypeShowInfo;
            timer = 0;
        }
        else if(stage == StageTypeShowInfo)
        {
            if(timer >= 2000)
            {
                stage = StageTypeRunning;
                timer = 0;
            }
        }
        else if(stage == StageTypeRunning)
        {
            if(end_detection())
            {
                stage = StageTypeEnd;
                timer = 0;
            }
            else
            {
                update_grid();
            }
        }
        else if(stage == StageTypeEnd)
        {
            update_grid();
            if(timer >= 5000)
            {
                if(mode == MODE_NEXT)
                {
                    pattern++;
                    pattern %= PatternTypeCycleMax;
                    stage = StageTypeInit;
                    timer = 0;
                }
                else if(mode == MODE_LOOP)
                {
                    stage = StageTypeInit;
                    timer = 0;
                }
                else // MODE_STOP
                {
                    // Do nothing
                    timer = 5000;
                }
            }
        }

        // Draw grid
        draw_grid();
        wrefresh(w_grid);
    }

    // Ask to end program
    wattron(w_status, COLOR_PAIR(COLOR_PAIR_STANDARD));
    mvwaddstr(w_status, 0, 0, " Press any key to quit");
    wattroff(w_status, COLOR_PAIR(COLOR_PAIR_STANDARD));
    wrefresh(w_status);
    wgetch(w_status);

    // End program
    endwin();
    exit(0);
}
