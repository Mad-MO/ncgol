
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

// TODO: New pattern "Octagon"
// TODO: New pattern "Tumbler"
// TODO: Add stopper for Glider guns
// TODO: Check for minimum grid size (8x8 Grid at least...)
// TODO: Add assert() from assert.h to check struct size from patterns

#include <curses.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <locale.h>
#include <ctype.h>
#include <time.h>
#include <getopt.h>
#include "grid.h"



// Define SW name and Version
#define SW_NAME "ncgol"
#define SW_VERS "v0.6"
#define AUTHOR  "domo"

#define SPEED_MAX  9
static uint8_t  speed;

static uint16_t grid_width;
static uint16_t grid_height;

WINDOW *w_grid_box;
WINDOW *w_grid;
WINDOW *w_status_box;
WINDOW *w_status;

static initpattern_t initpattern;

typedef enum
{
    STAGE_STARTUP,   // Show startup screen
    STAGE_STARTWAIT, // Timeout for startup window
    STAGE_INIT,      // Initialize grid
    STAGE_SHOWINFO,  // Show info for current pattern
    STAGE_RUNNING,   // Running simulation
    STAGE_END        // End of simulation has been reached
} stage_t;
static stage_t stage;

typedef enum
{
    // Styles which use two chars per cell
    STYLE_HASH,
    STYLE_BLOCK,

    // Styles which show two cells in one char
    STYLE_DOUBLE,

    // Braile style with 8 dots per char
    STYLE_BRAILLE,
    // ----------------
    STYLE_MAX
} style_t;
static style_t style;

typedef enum
{
    COLORS_STANDARD = 1,
    COLORS_LABEL,
    COLORS_VALUE,
    COLORS_LIVE_CELL,
    COLORS_TITLE,
    COLORS_PATTERN_INFO
} colors_t;

typedef enum
{
    AUTOMODE_NEXT, // When current pattern ends, switch to next pattern
    AUTOMODE_LOOP, // When current pattern ends, restart current pattern
    AUTOMODE_STOP, // When current pattern ends, stop the simulation
    // ----------------
    MODE_MAX
} automode_t;
static automode_t automode;

#define TIMEOUT_STARTWAIT 20000
#define TIMEOUT_SHOWINFO   2000
#define TIMEOUT_END        5000
static uint16_t timer;

#define COMMAND_KEYS_STR "Command keys:\n"                                   \
                         "  \'q\'                 End program\n"             \
                         "  \'Up\' and \'Down\'     Adjust speed\n"          \
                         "  \'0\'...\'9\'           Set speed directly\n"    \
                         "  \'Left\' and \'Right\'  Change pattern\n"        \
                         "  \'Space\'             Restart current pattern\n" \
                         "  \'s\'                 Change style\n"            \
                         "  \'m\'                 Change mode\n"             \
                         "  \'h\'                 Startupscreen\n"



// Function to initialize the User Interface
static void init_tui(void);

// Function to draw a string in a rounded frame for the pattern name
static void draw_str_in_frame(const char * str);

// Function to draw the grid on the canvas
static void draw_grid(void);

// Function to handle input events
static void handle_inputs(void);

// Function to handle one life cycle of the simulation
int main(int argc, char * argv[]);

// Handle the given commandline arguments
static void handle_args(int argc, char * argv[]);




// Function to initialize the User Interface
static void init_tui(void)
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
    init_pair(COLORS_STANDARD,     COLOR_WHITE,   COLOR_BLACK);   // Standard
    init_pair(COLORS_LABEL,        COLOR_GREEN,   COLOR_BLACK);   // Label
    init_pair(COLORS_VALUE,        COLOR_YELLOW,  COLOR_BLACK);   // Value
    init_pair(COLORS_LIVE_CELL,    COLOR_WHITE,   COLOR_BLACK);   // Live cell
    init_pair(COLORS_TITLE,        COLOR_CYAN,    COLOR_BLACK);   // Title
    init_pair(COLORS_PATTERN_INFO, COLOR_WHITE,   COLOR_MAGENTA); // Pattern info
    ESCDELAY = 1; // Set the delay for escape sequences

    // Create status window
    w_status_box = newwin(3, COLS, LINES-3, 0);
    box(w_status_box, 0, 0); // Draw a box around the screen
    wattron(w_status_box, COLOR_PAIR(COLORS_TITLE));
    mvwaddstr(w_status_box, 0, 3, " Status ");
    wattroff(w_status_box, COLOR_PAIR(0));
    wrefresh(w_status_box);
    w_status = newwin(1, COLS-2, LINES-2, 1);
    keypad(w_status, TRUE); // Enable special keys

    // Create grid window
    w_grid_box = newwin(LINES-3, COLS, 0, 0);
    box(w_grid_box, 0, 0); // Draw a box around the screen
    wattron(w_grid_box, COLOR_PAIR(COLORS_TITLE));
    mvwaddstr(w_grid_box, 0, 3, " Game of Life ");
    wattroff(w_grid_box, COLOR_PAIR(COLORS_TITLE));
    wrefresh(w_grid_box);
    w_grid = newwin(LINES-5, COLS-2, 1, 1);
    nodelay(w_grid, TRUE); // Non-blocking input
    keypad(w_grid, TRUE); // Enable special keys

    // Init
    if     (style == STYLE_BRAILLE)
    {
        grid_width  = getmaxx(w_grid) * 2;
        grid_height = getmaxy(w_grid) * 4;
    }
    else if(style >= STYLE_DOUBLE)
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
    grid_set_size(grid_width, grid_height);
}



// Function to draw a string in a rounded frame for the pattern name
static void draw_str_in_frame(const char * str)
{
    uint16_t len=strlen(str);

    // Calculate center position
    uint16_t x;
    uint16_t y;
    if((len+4) > getmaxx(w_grid))
    {
        x = 0;
    }
    else
    {
        x = (getmaxx(w_grid) - len) / 2 - 2;
    }
    y = (getmaxy(w_grid)) / 4;

    wattron(w_grid, A_BOLD | COLOR_PAIR(COLORS_PATTERN_INFO));
    for(uint16_t i=0; i<len+4; i++)
    {
        mvwaddch(w_grid, y+0, x+i, ' ');
        mvwaddch(w_grid, y+1, x+i, ' ');
        mvwaddch(w_grid, y+2, x+i, ' ');
    }
    mvwaddstr(w_grid, y+1, x+2, str);
    wattroff(w_grid, A_BOLD | COLOR_PAIR(COLORS_PATTERN_INFO));
}



// Function to draw the grid on the canvas
static void draw_grid(void)
{
    uint16_t x, y;
    char str[16];
    grid_t * grid = grid_get();

    // Draw grid to canvas
    wattron(w_grid, A_BOLD | COLOR_PAIR(COLORS_LIVE_CELL));
    for(x=0; x<grid_width; x++)
    {
        for(y=0; y<grid_height; y++)
        {
            if(style == STYLE_DOUBLE)
            {
                // Two dots per character
                if(grid[x][y] && grid[x][y + 1])
                {
                    // Both dots
                    #if(defined __linux__)
                        mvwaddstr(w_grid, y/2, x, "\u2588");
                    #else
                        mvwaddch(w_grid, y/2, x, ':');
                    #endif
                }
                else if(grid[x][y])
                {
                    // Upper dot
                    #if(defined __linux__)
                        mvwaddstr(w_grid, y/2, x, "\u2580");
                    #else
                        mvwaddch(w_grid, y/2, x, '\'');
                    #endif
                }
                else if(grid[x][y + 1])
                {
                    // Lower dot
                    #if(defined __linux__)
                        mvwaddstr(w_grid, y/2, x, "\u2584");
                    #else
                        mvwaddch(w_grid, y/2, x, '.');
                    #endif
                }
                else
                {
                    mvwaddch(w_grid, y/2, x, ' ');
                }
                y++;
            }
            else if(style == STYLE_BRAILLE)
            {
                // The braille characters allows the usage of 8 dots per character
                uint16_t braille = 0;
                wchar_t braille_char;
                char braille_str[4] = {0};

                // Convert grid to braille unicode character
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
                    if     (style == STYLE_BLOCK)
                        #if(defined __linux__)
                            mvwaddstr(w_grid, y, x*2, "\u2588\u2588"); // Two full blocks -> Looks best on a linux terminal which leaves no horizontal space between the blocks
                        #else
                            mvwaddstr(w_grid, y, x*2, "\u2588\u258a"); // Full block and 3/4 block -> Looks best on a mac terminal which leaves a little horizontal space between the blocks
                        #endif
                    else          // STYLE_HASH
                        mvwaddstr(w_grid, y, x*2, "# ");               // #  -> Looks best on a terminal which has problems with unicode characters
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
    if((stage == STAGE_STARTUP) || (stage == STAGE_STARTWAIT))
    {
        wmove(w_grid, 0, 0);
        waddstr(w_grid, "\n");
        wattron(w_grid, A_BOLD | A_REVERSE);
        waddstr(w_grid, " A ncurses based Simulation for Conways \"Game of Life\" \n");
        wattroff(w_grid, A_BOLD | A_REVERSE);
        waddstr(w_grid, "\n");
        wprintw(w_grid, "Maximum grid size:    %ux%u\n", GRID_WIDTH_MAX, GRID_HEIGHT_MAX);
        waddstr(w_grid, "\n");
        waddstr(w_grid, COMMAND_KEYS_STR);
    }

    // Handle pattern info
    if(stage == STAGE_SHOWINFO)
    {
        draw_str_in_frame(grid_get_initpattern_str(initpattern));
    }

    // Handle end message
    if(stage == STAGE_END)
    {
        draw_str_in_frame("Simulation End");
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
            wattron(w_status, COLOR_PAIR(COLORS_LABEL));
            waddstr(w_status, str_label);
            wattron(w_status, COLOR_PAIR(COLORS_VALUE));
            waddstr(w_status, str_value);
        }

        // Cycles
        strcpy(str_label, " Cycles:");
        sprintf(str_value, "%3u", grid_get_cycle_counter());
        if((getcurx(w_status)+strlen(str_label)+strlen(str_value)) < width)
        {
            wattron(w_status, COLOR_PAIR(COLORS_LABEL));
            waddstr(w_status, str_label);
            wattron(w_status, COLOR_PAIR(COLORS_VALUE));
            waddstr(w_status, str_value);
        }

        // Cells
        strcpy(str_label, " Cells:");
        sprintf(str_value, "%3u", grid_get_cells_alive());
        if((getcurx(w_status)+strlen(str_label)+strlen(str_value)) < width)
        {
            wattron(w_status, COLOR_PAIR(COLORS_LABEL));
            waddstr(w_status, str_label);
            wattron(w_status, COLOR_PAIR(COLORS_VALUE));
            waddstr(w_status, str_value);
        }

        // Speed
        strcpy(str_label, " Speed:");
        sprintf(str_value, "%u", speed);
        if((getcurx(w_status)+strlen(str_label)+strlen(str_value)) < width)
        {
            wattron(w_status, COLOR_PAIR(COLORS_LABEL));
            waddstr(w_status, str_label);
            wattron(w_status, COLOR_PAIR(COLORS_VALUE));
            waddstr(w_status, str_value);
        }

        // Pattern
        strcpy(str_label, " Pattern:");
        strcpy(str_value, grid_get_initpattern_str(initpattern));
        if((getcurx(w_status)+strlen(str_label)+strlen(str_value)) < width)
        {
            wattron(w_status, COLOR_PAIR(COLORS_LABEL));
            waddstr(w_status, str_label);
            wattron(w_status, COLOR_PAIR(COLORS_VALUE));
            waddstr(w_status, str_value);
        }

        // Style
        strcpy(str_label, " Style:");
        if     (style == STYLE_HASH)
            strcpy(str_value, "Hash");
        else if(style == STYLE_BLOCK)
            strcpy(str_value, "Block");
        else if(style == STYLE_DOUBLE)
            strcpy(str_value, "Double");
        else if(style == STYLE_BRAILLE)
            strcpy(str_value, "Braille");
        else
            strcpy(str_value, "?");
        if((getcurx(w_status)+strlen(str_label)+strlen(str_value)) < width)
        {
            wattron(w_status, COLOR_PAIR(COLORS_LABEL));
            waddstr(w_status, str_label);
            wattron(w_status, COLOR_PAIR(COLORS_VALUE));
            waddstr(w_status, str_value);
        }

        // Mode
        strcpy(str_label, " Mode:");
        if     (automode == AUTOMODE_NEXT)
            strcpy(str_value, "Next");
        else if(automode == AUTOMODE_LOOP)
            strcpy(str_value, "Loop");
        else if(automode == AUTOMODE_STOP)
            strcpy(str_value, "Stop");
        else
            strcpy(str_value, "?");
        if((getcurx(w_status)+strlen(str_label)+strlen(str_value)) < width)
        {
            wattron(w_status, COLOR_PAIR(COLORS_LABEL));
            waddstr(w_status, str_label);
            wattron(w_status, COLOR_PAIR(COLORS_VALUE));
            waddstr(w_status, str_value);
        }

        // Remember position and clear rest of line
        waddstr(w_status, "   ");
        pos = getcurx(w_status);
        for(uint16_t i=pos; i<width; i++)
            waddch(w_status, ' ');

        // Author
        char author[] = "by "AUTHOR;
        if((width-strlen(author)-1) > pos)
        {
            wattron(w_status, COLOR_PAIR(COLORS_STANDARD));
            mvwaddstr(w_status, 0, width-strlen(author)-1, author);
        }

        // SW Version
        if((width-strlen(author)-1-strlen(SW_VERS)-1) > pos)
        {
            wattron(w_status, COLOR_PAIR(COLORS_VALUE));
            mvwaddstr(w_status, 0, width-strlen(author)-1-strlen(SW_VERS)-1, SW_VERS);
        }

        // SW Name
        if((width-strlen(author)-1-strlen(SW_VERS)-1-strlen(SW_NAME)-1) > pos)
        {
            wattron(w_status, COLOR_PAIR(COLORS_STANDARD));
            mvwaddstr(w_status, 0, width-strlen(author)-1-strlen(SW_VERS)-1-strlen(SW_NAME)-1, SW_NAME);
        }
    }

    wattroff(w_status, COLOR_PAIR(COLORS_VALUE));
    wrefresh(w_status);
}



// Function to handle input events
static void handle_inputs(void)
{
    int key = wgetch(w_grid);
    if(key==KEY_RESIZE)
    {
        init_tui();
    }
    else if(tolower(key) == 'q')
    {
        endwin();
        exit(0);
    }
    else if(key == KEY_UP)
    {
        if(speed < SPEED_MAX) speed++;
    }
    else if(key == KEY_DOWN)
    {
        if(speed > 0) speed--;
    }
    else if((key>='0') && (key<='9'))
    {
        speed = key - '0';
    }
    else if(key == KEY_LEFT)
    {
        if(initpattern == 0) initpattern = INITPATTERN_CYCLEMAX - 1;
        else             initpattern--;
        stage = STAGE_INIT;
    }
    else if(key == KEY_RIGHT)
    {
        initpattern++;
        initpattern %= INITPATTERN_CYCLEMAX;
        stage = STAGE_INIT;
    }
    else if(key == ' ')
    {
        stage   = STAGE_INIT;
    }
    else if(tolower(key) == 'c')
    {
        style++;
        style %= STYLE_MAX;
        init_tui();
    }
    else if(tolower(key) == 'm')
    {
        automode++;
        automode %= MODE_MAX;
    }
    else if(tolower(key) == 'h')
    {
        stage       = STAGE_STARTUP;
    }
    if(key==27) // ESC
    {
        if     (stage == STAGE_STARTWAIT)
        {
            stage = STAGE_INIT;
        }
        else if(stage == STAGE_SHOWINFO)
        {
            stage = STAGE_RUNNING;
        }
        else if(stage == STAGE_END)
        {
            timer = TIMEOUT_END;
        }
    }
    else
    {
        // Do nothing
    }
}



// Function to handle one life cycle of the simulation
int main(int argc, char * argv[])
{
    // Initialize variables
    initpattern = INITPATTERN_RANDOM;
    speed       = 3;
    automode    = AUTOMODE_NEXT;
    style       = STYLE_HASH;

    // Handle commandline arguments
    handle_args(argc, argv);

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
        uint16_t ticks;

        // Handle passed time
        clock_gettime(CLOCK_REALTIME, &ts);
        ticks = ts.tv_nsec / 1000000; // ticks = 0...999
        timer += (ticks - last_ticks + 1000) % 1000;
        last_ticks = ticks;

        // Handle input
        handle_inputs();

        // Handle speed
        if     (speed == 0) usleep(500000); // Stop
        else if(speed == 1) usleep(500000); // ~2 Hz
        else if(speed == 2) usleep(100000);
        else if(speed == 3) usleep( 50000);
        else if(speed == 4) usleep( 10000);
        else if(speed == 5) usleep(  5000);
        else if(speed == 6) usleep(  1000);
        else if(speed == 7) usleep(   500);
        else if(speed == 8) usleep(   100);
        else                usleep(     0); // As fast as possible

        // Handle different patterns and update grid
        if     (stage == STAGE_STARTUP)
        {
            grid_init(INITPATTERN_CLEAR);
            stage = STAGE_STARTWAIT;
            timer = 0;
        }
        else if(stage == STAGE_STARTWAIT)
        {
            if(timer >= TIMEOUT_STARTWAIT)
            {
                stage = STAGE_INIT;
                timer = 0;
            }
        }
        else if(stage == STAGE_INIT)
        {
            grid_init(initpattern);
            stage = STAGE_SHOWINFO;
            timer = 0;
        }
        else if(stage == STAGE_SHOWINFO)
        {
            if(timer >= TIMEOUT_SHOWINFO)
            {
                stage = STAGE_RUNNING;
                timer = 0;
            }
        }
        else if(stage == STAGE_RUNNING)
        {
            if(speed > 0)
            {
                grid_update();
            }
            if(grit_end_detected())
            {
                stage = STAGE_END;
                timer = 0;
            }
        }
        else if(stage == STAGE_END)
        {
            if(speed > 0)
            {
              grid_update();
            }
            if(timer >= TIMEOUT_END)
            {
                if(automode == AUTOMODE_NEXT)
                {
                    initpattern++;
                    initpattern %= INITPATTERN_CYCLEMAX;
                    stage = STAGE_INIT;
                    timer = 0;
                }
                else if(automode == AUTOMODE_LOOP)
                {
                    stage = STAGE_INIT;
                    timer = 0;
                }
                else // AUTOMODE_STOP
                {
                    // Do nothing
                    timer = TIMEOUT_END;
                }
            }
        }

        // Draw grid
        draw_grid();
        wrefresh(w_grid);
    }

    // Ask to end program
    wattron(w_status, COLOR_PAIR(COLORS_STANDARD));
    mvwaddstr(w_status, 0, 0, " Press any key to quit");
    wattroff(w_status, COLOR_PAIR(COLORS_STANDARD));
    wrefresh(w_status);
    wgetch(w_status);

    // End program
    endwin();
    exit(0);
}



void handle_args(int argc, char * argv[])
{
    // Handle commandline arguments
    // https://www.gnu.org/software/libc/manual/html_node/Getopt-Long-Options.html
    while (1)
    {
        static struct option long_options[] =
        {
            {"charstyle", required_argument, 0, 'c'},
            {"help",      no_argument,       0, 'h'},
            {"init",      required_argument, 0, 'i'},
            {"mode",      required_argument, 0, 'm'},
            {"nowait",    no_argument,       0, 'n'},
            {"speed",     required_argument, 0, 's'},
            {"version",   no_argument,       0, 'v'},
            // -----------------------------------
            {0,           0,                 0,   0}
        };

        int c = getopt_long(argc, argv, "c:hi:m:ns:v", long_options, 0);

        // Detect the end of the options
        if (c == -1)
            break;

        switch(c)
        {
            case 'c':
            {
                if     (strcmp(optarg, "hash") == 0)
                    style = STYLE_HASH;
                else if(strcmp(optarg, "block") == 0)
                    style = STYLE_BLOCK;
                else if(strcmp(optarg, "double") == 0)
                    style = STYLE_DOUBLE;
                else if(strcmp(optarg, "braille") == 0)
                    style = STYLE_BRAILLE;
                else
                {
                    printf("Invalid charstyle value: %s\n", optarg);
                    printf("Character style must be one of: hash, block, double, braille\n");
                    exit(1);
                }
                break;
            }

            case 'h':
            {
                printf("Usage:\n");
                printf("  %s [options]\n", SW_NAME);
                printf("\n");
                printf("%s - ncurses Game of Life %s by %s\n", SW_NAME, SW_VERS, AUTHOR);
                printf("\n");
                printf("Options:\n");
                printf("  -c, --charstyle  Set character style (hash, block, double, braille)\n");
                printf("  -h, --help       This Help\n");
                printf("  -i, --init       Set initial pattern\n"
                       "                   (random, conway, stilllifes, oscillators, spaceships,\n"
                       "                   gosper, simkin, pentomino, diehard, acorn, blockengine1,\n"
                       "                   blockengine2, doubleblockengine, ilove8bit)\n");
                printf("  -m, --mode       Set mode (next, loop, stop)\n");
                printf("  -n, --nowait     Start without Startupscreen\n");
                printf("  -s, --speed      Set speed (0-9)\n");
                printf("\n");
                printf(COMMAND_KEYS_STR);
                exit(0);
            }

            case 'i':
            {
                if     (strcmp(optarg, "random") == 0)
                    initpattern = INITPATTERN_RANDOM;
                else if(strcmp(optarg, "conway") == 0)
                    initpattern = INITPATTERN_CONWAY;
                else if(strcmp(optarg, "stilllifes") == 0)
                    initpattern = INITPATTERN_STILLLIFES;
                else if(strcmp(optarg, "oscillators") == 0)
                    initpattern = INITPATTERN_OSCILLATORS;
                else if(strcmp(optarg, "spaceships") == 0)
                    initpattern = INITPATTERN_SPACESHIPS;
                else if(strcmp(optarg, "gosper") == 0)
                    initpattern = INITPATTERN_GOSPER_GLIDERGUN;
                else if(strcmp(optarg, "simkin") == 0)
                    initpattern = INITPATTERN_SIMKIN_GLIDERGUN;
                else if(strcmp(optarg, "pentomino") == 0)
                    initpattern = INITPATTERN_PENTOMINO;
                else if(strcmp(optarg, "diehard") == 0)
                    initpattern = INITPATTERN_DIEHARD;
                else if(strcmp(optarg, "acorn") == 0)
                    initpattern = INITPATTERN_ACORN;
                else if(strcmp(optarg, "blockengine1") == 0)
                    initpattern = INITPATTERN_BLOCKENGINE1;
                else if(strcmp(optarg, "blockengine2") == 0)
                    initpattern = INITPATTERN_BLOCKENGINE2;
                else if(strcmp(optarg, "doubleblockengine") == 0)
                    initpattern = INITPATTERN_DOUBLEBLOCKENGINE;
                else if(strcmp(optarg, "ilove8bit") == 0)
                    initpattern = INITPATTERN_ILOVE8BIT;
                else
                {
                    printf("Invalid init value: %s\n", optarg);
                    printf("Init must be one of: random, conway, stilllifes, oscillators, spaceships, gosper, simkin, pentomino, diehard, acorn, blockengine1, blockengine2, doubleblockengine, ilove8bit\n");
                    exit(1);
                }
                break;
            }

            case 'm':
            {
                if     (strcmp(optarg, "next") == 0)
                    automode = AUTOMODE_NEXT;
                else if(strcmp(optarg, "loop") == 0)
                    automode = AUTOMODE_LOOP;
                else if(strcmp(optarg, "stop") == 0)
                    automode = AUTOMODE_STOP;
                else
                {
                    printf("Invalid mode value: %s\n", optarg);
                    printf("Mode must be one of: next, loop, stop\n");
                    exit(1);
                }
                break;
            }

            case 'n':
            {
                stage = STAGE_INIT;
                break;
            }

            case 's':
            {
                int val = atoi(optarg);
                if(val <= SPEED_MAX)
                {
                    speed = val;
                }
                else
                {
                    printf("Invalid speed value: %s\n", optarg);
                    printf("Speed must be between 0 and 9\n");
                    exit(1);
                }
                break;
            }

            case 'v':
            {
                printf("%s - ncurses Game of Life %s by %s\n", SW_NAME, SW_VERS, AUTHOR);
                exit(0);
            }

            case '?': // getopt_long() already printed an error message
            default:
            {

                exit(1);
            }
        }
    }

    // Print any remaining command line arguments (not options)
    if (optind < argc)
    {
        printf ("%s: unrecognized argument ", SW_NAME);
        while (optind < argc)
        {
            printf ("\'%s\' ", argv[optind++]);
        }
        printf("\n");
        exit(1);
    }
}
