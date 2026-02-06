
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

// TODO: Improve thread performance: Try start smaller threads and start next thread when the last one is finished
// TODO: Improve key reaction time on lowest speeds or during end detected
// TODO: Benchmark (calc without drawing of very large grid) --benchmark / -b
// TODO: Add assert() from assert.h to check struct size from patterns
// TODO: Add man page
// TODO: Prepare for linux package

#include <curses.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <locale.h>
#include <ctype.h>
#include <time.h>
#include <getopt.h>
#include <pthread.h>
#include "config.h"
#include "grid.h"
#include "debug_output.h"

// Define SW name and Version
#define SW_NAME       "ncgol"
#define SW_VERS       "v0.6"
#define AUTHOR_SHORT  "M. Ochs"
#define AUTHOR_LONG   "Martin Ochs"

static uint8_t grid_draw[GRID_WIDTH_MAX][GRID_HEIGHT_MAX];

#define SPEED_MAX  9
static uint8_t  speed;
static float hz;

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
    CHARSTYLE_HASH,    // Charstyle which uses '#' and two chars per cell
    CHARSTYLE_BLOCK,   // Charstyle which use unicode blocks in two chars per cell
    CHARSTYLE_DOUBLE,  // Charstyle which shows two cells in one char
    CHARSTYLE_BRAILLE, // Braile charstyle with 8 cells per char
    // ----------------
    CHARSTYLE_MAX
} charstyle_t;
static charstyle_t charstyle;

// Text strings for the charstyle_t enum
static const char * charstyle_str[][2] =
{
    {"hash",    "Hash char"},
    {"block",   "Blocks"},
    {"double",  "Double 1x2"},
    {"braille", "Braille 2x4"}
};

typedef enum
{
    COLORS_DEFAULT = 0, // Already defined by ncurses with shell colors
    COLORS_STANDARD,
    COLORS_LABEL,
    COLORS_VALUE,
    COLORS_INFO,
    COLORS_LIVE_CELL,
    COLORS_TITLE,
    COLORS_PATTERN_INFO,
    COLORS_ERROR
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

// Text strings for the automode_t enum
static const char * automode_str[][2] =
{
    {"next", "Next pattern"},
    {"loop", "Loop current"},
    {"stop", "Stop after"}
};

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
                         "  \'c\'                 Change Charstyle\n"        \
                         "  \'m\'                 Change mode\n"             \
                         "  \'h\'                 Startupscreen\n"



// Function to initialize the User Interface
static void tui_init(void);

// Function to determine if grid is too small
static uint8_t grid_too_small(void);

// Function to draw a string in a rounded frame for the pattern name
static void draw_str_in_frame(const char * str);

// Function to update the grid on the canvas (starts thread with tui_draw)
static void tui_update(void);

// Function to draw the grid on the canvas
static void * tui_draw(void * args);

// Function to handle input events
static void handle_inputs(void);

// Function to handle one life cycle of the simulation
int main(int argc, char * argv[]);

// Handle the given commandline arguments
static void handle_args(int argc, char * argv[]);



// Function to initialize the User Interface
static void tui_init(void)
{
    // Init debug output
    #if (WITH_DEBUG_OUTPUT)
        debug_init();
    #endif

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
    use_default_colors();                   // Use shell colors
    NCURSES_COLOR_T fg, bg;
    pair_content(COLORS_DEFAULT, &fg, &bg); // Get default colors from shell
    init_pair(COLORS_STANDARD,     fg,            bg);            // Standard
    init_pair(COLORS_LABEL,        fg,            bg);            // Label
    init_pair(COLORS_VALUE,        COLOR_YELLOW,  bg);            // Value
    init_pair(COLORS_INFO,         COLOR_CYAN,    bg);            // Info
    init_pair(COLORS_LIVE_CELL,    fg,            bg);            // Live cell
    init_pair(COLORS_TITLE,        COLOR_CYAN,    bg);            // Title
    init_pair(COLORS_PATTERN_INFO, COLOR_WHITE,   COLOR_MAGENTA); // Pattern info
    init_pair(COLORS_ERROR,        COLOR_WHITE,   COLOR_RED);     // Error
    ESCDELAY = 1; // Set the delay for escape sequences

    // Create status window
    w_status_box = newwin(3, COLS, LINES-3, 0);
    box(w_status_box, 0, 0); // Draw a box around the screen
    wattron(w_status_box, COLOR_PAIR(COLORS_TITLE));
    mvwaddstr(w_status_box, 0, 3, " Status ");
    wattroff(w_status_box, COLOR_PAIR(0));
    wrefresh(w_status_box);
    w_status = newwin(1, COLS-2, LINES-2, 1);
    nodelay(w_status, TRUE); // Non-blocking input
    keypad(w_status, TRUE); // Enable special keys
    wrefresh(w_status);

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
    wrefresh(w_grid);

    // Init
    if     (charstyle == CHARSTYLE_BRAILLE)
    {
        grid_width  = getmaxx(w_grid) * 2;
        grid_height = getmaxy(w_grid) * 4;
    }
    else if(charstyle >= CHARSTYLE_DOUBLE)
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
    memset(grid_draw, 0, sizeof(grid_draw));

    #if (WITH_DEBUG_OUTPUT)
        debug_printf("Grid size: %ux%u\n", grid_width, grid_height);
    #endif
}



// Function to determine if grid is too small
static uint8_t grid_too_small(void)
{
    return ((grid_width < 16) || (grid_height < 8));
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
static void * tui_draw(void * args)
{
    uint16_t x, y;
    char str[16];

    // Draw grid to canvas
    wattron(w_grid, A_BOLD | COLOR_PAIR(COLORS_LIVE_CELL));
    for(x=0; x<grid_width; x++)
    {
        for(y=0; y<grid_height; y++)
        {
            if(charstyle == CHARSTYLE_DOUBLE)
            {
                // Two dots per character
                if(grid_draw[x][y] && grid_draw[x][y + 1])
                {
                    // Both dots
                    #if(defined __linux__)
                        mvwaddstr(w_grid, y/2, x, "\u2588");
                    #else
                        mvwaddch(w_grid, y/2, x, ':');
                    #endif
                }
                else if(grid_draw[x][y])
                {
                    // Upper dot
                    #if(defined __linux__)
                        mvwaddstr(w_grid, y/2, x, "\u2580");
                    #else
                        mvwaddch(w_grid, y/2, x, '\'');
                    #endif
                }
                else if(grid_draw[x][y + 1])
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
            else if(charstyle == CHARSTYLE_BRAILLE)
            {
                // The braille characters allows the usage of 8 dots per character
                uint16_t braille = 0;
                wchar_t braille_char;
                char braille_str[4] = {0};

                // Convert grid to braille unicode character
                if(grid_draw[x+0][y+0]) {braille |= 0x01;}
                if(grid_draw[x+0][y+1]) {braille |= 0x02;}
                if(grid_draw[x+0][y+2]) {braille |= 0x04;}
                if(grid_draw[x+0][y+3]) {braille |= 0x40;}
                if(grid_draw[x+1][y+0]) {braille |= 0x08;}
                if(grid_draw[x+1][y+1]) {braille |= 0x10;}
                if(grid_draw[x+1][y+2]) {braille |= 0x20;}
                if(grid_draw[x+1][y+3]) {braille |= 0x80;}
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
                if(grid_draw[x][y])
                {
                    if     (charstyle == CHARSTYLE_BLOCK)
                        #if(defined __linux__)
                            mvwaddstr(w_grid, y, x*2, "\u2588\u2588"); // Two full blocks -> Looks best on a linux terminal which leaves no horizontal space between the blocks
                        #else
                            mvwaddstr(w_grid, y, x*2, "\u2588\u258a"); // Full block and 3/4 block -> Looks best on a mac terminal which leaves a little horizontal space between the blocks
                        #endif
                    else          // CHARSTYLE_HASH
                        mvwaddstr(w_grid, y, x*2, "# ");               // #  -> Looks best on a terminal which has problems with unicode characters
                }
                else
                {
                    mvwaddstr(w_grid, y, x*2, "  ");
                }
            }
        }
    }
    wattroff(w_grid, A_BOLD | COLOR_PAIR(COLORS_LIVE_CELL));

    // Handle grid screen messages
    if((stage == STAGE_STARTUP) || (stage == STAGE_STARTWAIT))
    {
        // Handle startup screen
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
    else if(stage == STAGE_SHOWINFO)
    {
        // Handle pattern info
        draw_str_in_frame(grid_get_initpattern_long_str(initpattern));
    }
    else if(stage == STAGE_END)
    {
        // Handle end message
        draw_str_in_frame("Simulation End");
    }

    // Handle status line
    {
        // Full line ==> "Grid:2500x1000 Cycles:123456 Cells:1500000 Speed:9 (100 Hz) Pattern:Random (fire) Charstyle:Braille 2x4 Mode:Next pattern Cores:8  ncgol v0.1 by Martin Ochs"
        char str_label[20];
        char str_value[30];
        uint16_t pos   = 0;
        uint16_t width = getmaxx(w_status);
        wmove(w_status, 0, 0);

        // Grid
        strcpy(str_label, " Grid:");
        sprintf(str_value, "%ux%u", grid_width, grid_height);
        if((getcurx(w_status)+strlen(str_label)+strlen(str_value)) < width)
        {
            if(grid_too_small())
            {
                wattron(w_status, COLOR_PAIR(COLORS_LABEL));
                waddstr(w_status, str_label);
                wattron(w_status, A_BOLD | COLOR_PAIR(COLORS_ERROR));
                waddstr(w_status, str_value);
                waddstr(w_status, " is too small");
                wattroff(w_status, A_BOLD | COLOR_PAIR(COLORS_ERROR));
            }
            else
            {
                wattron(w_status, COLOR_PAIR(COLORS_LABEL));
                waddstr(w_status, str_label);
                wattron(w_status, COLOR_PAIR(COLORS_VALUE));
                waddstr(w_status, str_value);
                wattroff(w_status, COLOR_PAIR(COLORS_VALUE));
            }
        }

        if(!grid_too_small())
        {
            // Cycles
            strcpy(str_label, " Cycles:");
            sprintf(str_value, "%3u", grid_get_cycle_counter());
            if((getcurx(w_status)+strlen(str_label)+strlen(str_value)) < width)
            {
                wattron(w_status, COLOR_PAIR(COLORS_LABEL));
                waddstr(w_status, str_label);
                wattron(w_status, COLOR_PAIR(COLORS_VALUE));
                waddstr(w_status, str_value);
                wattroff(w_status, COLOR_PAIR(COLORS_VALUE));
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
                wattroff(w_status, COLOR_PAIR(COLORS_VALUE));
            }

            // Speed
            strcpy(str_label, " Speed:");
            if(hz <= 9.9)
            {
                sprintf(str_value, "%u (%0.1f Hz)", speed, hz);
            }
            else
            {
                sprintf(str_value, "%u (%0.0f Hz)", speed, hz);
            }
            if((getcurx(w_status)+strlen(str_label)+strlen(str_value)) < width)
            {
                wattron(w_status, COLOR_PAIR(COLORS_LABEL));
                waddstr(w_status, str_label);
                wattron(w_status, COLOR_PAIR(COLORS_VALUE));
                waddstr(w_status, str_value);
                wattroff(w_status, COLOR_PAIR(COLORS_VALUE));
            }

            // Pattern
            strcpy(str_label, " Pattern:");
            if(strlen(grid_get_initpattern_long_str(initpattern)) < sizeof(str_value))
            {
                strcpy(str_value, grid_get_initpattern_long_str(initpattern));
            }
            else
            {
                strcpy(str_value, "?");
            }
            if((getcurx(w_status)+strlen(str_label)+strlen(str_value)) < width)
            {
                wattron(w_status, COLOR_PAIR(COLORS_LABEL));
                waddstr(w_status, str_label);
                wattron(w_status, COLOR_PAIR(COLORS_VALUE));
                waddstr(w_status, str_value);
                wattroff(w_status, COLOR_PAIR(COLORS_VALUE));
            }

            // Style
            strcpy(str_label, " Charstyle:");
            if(charstyle < CHARSTYLE_MAX)
            {
                strcpy(str_value, charstyle_str[charstyle][1]);
            }
            else
            {
                strcpy(str_value, "?");
            }
            if((getcurx(w_status)+strlen(str_label)+strlen(str_value)) < width)
            {
                wattron(w_status, COLOR_PAIR(COLORS_LABEL));
                waddstr(w_status, str_label);
                wattron(w_status, COLOR_PAIR(COLORS_VALUE));
                waddstr(w_status, str_value);
                wattroff(w_status, COLOR_PAIR(COLORS_VALUE));
            }

            // Mode
            strcpy(str_label, " Mode:");
            if(automode < MODE_MAX)
            {
                strcpy(str_value, automode_str[automode][1]);
            }
            else
            {
                strcpy(str_value, "?");
            }
            if((getcurx(w_status)+strlen(str_label)+strlen(str_value)) < width)
            {
                wattron(w_status, COLOR_PAIR(COLORS_LABEL));
                waddstr(w_status, str_label);
                wattron(w_status, COLOR_PAIR(COLORS_VALUE));
                waddstr(w_status, str_value);
                wattroff(w_status, COLOR_PAIR(COLORS_VALUE));
            }

            // Cores
            strcpy(str_label, " Cores:");
            sprintf(str_value, "%i", grid_get_cpu_cores());
            if((getcurx(w_status)+strlen(str_label)+strlen(str_value)) < width)
            {
                wattron(w_status, COLOR_PAIR(COLORS_LABEL));
                waddstr(w_status, str_label);
                wattron(w_status, COLOR_PAIR(COLORS_VALUE));
                waddstr(w_status, str_value);
                wattroff(w_status, COLOR_PAIR(COLORS_VALUE));
            }

            // Remember position and clear rest of line
            waddstr(w_status, "   ");
            pos = getcurx(w_status);
            for(uint16_t i=pos; i<width; i++)
                waddch(w_status, ' ');

            // Author
            char author[] = "by "AUTHOR_LONG;
            if((width-strlen(author)-1) > pos)
            {
                wattron(w_status, COLOR_PAIR(COLORS_STANDARD));
                mvwaddstr(w_status, 0, width-strlen(author)-1, author);
                wattroff(w_status, COLOR_PAIR(COLORS_STANDARD));
            }
            else
            {
                strcpy(author, "by "AUTHOR_SHORT);
                if((width-strlen(author)-1) > pos)
                {
                    wattron(w_status, COLOR_PAIR(COLORS_STANDARD));
                    mvwaddstr(w_status, 0, width-strlen(author)-1, author);
                    wattroff(w_status, COLOR_PAIR(COLORS_STANDARD));
                }
            }

            // SW Version
            if((width-strlen(author)-1-strlen(SW_VERS)-1) > pos)
            {
                wattron(w_status, COLOR_PAIR(COLORS_INFO));
                mvwaddstr(w_status, 0, width-strlen(author)-1-strlen(SW_VERS)-1, SW_VERS);
                wattroff(w_status, COLOR_PAIR(COLORS_INFO));
            }

            // SW Name
            if((width-strlen(author)-1-strlen(SW_VERS)-1-strlen(SW_NAME)-1) > pos)
            {
                wattron(w_status, COLOR_PAIR(COLORS_STANDARD));
                mvwaddstr(w_status, 0, width-strlen(author)-1-strlen(SW_VERS)-1-strlen(SW_NAME)-1, SW_NAME);
                wattroff(w_status, COLOR_PAIR(COLORS_STANDARD));
            }
        }
    }

    wattroff(w_status, COLOR_PAIR(COLORS_VALUE));

    return NULL;
}



// Function to update the grid on the canvas (starts thread with tui_draw)
static void tui_update(void)
{
    static pthread_t thread;

    pthread_join(thread, NULL); // Wait for last thread to finish -> Should be done by now, but just in case
    wrefresh(w_grid);           // Refresh window -> This has to be done outside of the thread!
    wrefresh(w_status);
    memcpy(grid_draw, grid_get(), sizeof(grid_draw));
    if(pthread_create(&thread, NULL, tui_draw, NULL)) // During this drawing no wrefresh() on w_grid should be called (Caution: getch() in handle_inputs() is also a wrefresh()!)
    {
        endwin();
        exit(1);
    }
}



// Function to handle input events
static void handle_inputs(void)
{
    int key = wgetch(w_status);
    if(key==KEY_RESIZE)
    {
        tui_init();
        stage = STAGE_INIT;
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
        stage = STAGE_INIT;
    }
    else if(tolower(key) == 'c')
    {
        charstyle++;
        charstyle %= CHARSTYLE_MAX;
        tui_init();
    }
    else if(tolower(key) == 'm')
    {
        automode++;
        automode %= MODE_MAX;
    }
    else if(tolower(key) == 'h')
    {
        stage = STAGE_STARTUP;
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
    charstyle       = CHARSTYLE_HASH;

    // Handle commandline arguments
    handle_args(argc, argv);

    // Initialize ncurses and grid
    tui_init();

    // Init
    static uint16_t last_systime_ms;
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    last_systime_ms = ts.tv_nsec / 1000000;

    // Mainloop
    while(1)
    {
        uint16_t systime_ms;
        uint16_t ticks; // Passed time in milliseconds for last cycle

        // Handle passed time
        clock_gettime(CLOCK_REALTIME, &ts);
        systime_ms = ts.tv_nsec / 1000000;
        ticks = (systime_ms - last_systime_ms + 1000) % 1000;
        timer += ticks;
        last_systime_ms = systime_ms;

        // Handle input
        handle_inputs();

        // Handle speed
        if     (speed == 0)         usleep(500000); // Stop
        else if(speed == 1)         usleep(500000); // ~2 Hz
        else if(speed == 2)         usleep(100000);
        else if(speed == 3)         usleep( 50000);
        else if(speed == 4)         usleep( 10000);
        else if(speed == 5)         usleep(  5000);
        else if(speed == 6)         usleep(  1000);
        else if(speed == 7)         usleep(   500);
        else if(speed == 8)         usleep(   100);
        else                        /*Do nothing*/; // As fast as possible

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
            if(grid_too_small())
            {
                grid_init(INITPATTERN_CLEAR);
            }
            else
            {
                grid_init(initpattern);
                stage = STAGE_SHOWINFO;
            }
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
            if(grid_end_detected())
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

        // Measure Hz
        {
            static uint16_t hz_timer = 0;
            static uint16_t last_cycle_counter = 0;
            uint16_t cycles = grid_get_cycle_counter() - last_cycle_counter;
            hz_timer += ticks;
            if(    ((hz_timer >=  250) && (cycles >= 50)) // Above 200 Hz 4 measurements per second
                || ((hz_timer >=  500) && (cycles >= 10)) // Above  20 Hz 2 measurements per second
                || ((hz_timer >= 1000) && (cycles >=  3)) // Below  20 Hz 1 measurement per second
                ||  (hz_timer >= 2000)
              )
            {
                if(last_cycle_counter > grid_get_cycle_counter())
                {
                    hz = 0;
                }
                else
                {
                    hz = (float)(cycles * 1000) / (float)hz_timer;
                }
                last_cycle_counter = grid_get_cycle_counter();
                hz_timer = 0;
            }
        }

        // Show debug time measurement
        #if (WITH_DEBUG_OUTPUT)
            //debug_time_stop(DEBUG_TIME1);
            //debug_printf(" T1:%0.1f T2:%0.1f T3:%0.1f\n", (float)debug_time_get(DEBUG_TIME1)/1000, (float)debug_time_get(DEBUG_TIME2)/1000, (float)debug_time_get(DEBUG_TIME3)/1000);
            //debug_time_start(DEBUG_TIME1);
        #endif

        // Draw grid -> Reduce drawing to given Hz, because it is not necessary to draw the grid faster than the display can handle
        {
            #define DRAW_GRID_HZ 60
            static uint16_t draw_timer = 0;
            draw_timer += ticks;
            if(draw_timer >= (1000 / DRAW_GRID_HZ))
            {
                draw_timer -= (1000 / DRAW_GRID_HZ);
                tui_update();
            }
        }
    }

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
            // --------------------------------------
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
                charstyle = CHARSTYLE_MAX;
                for(int i=0; i<CHARSTYLE_MAX; i++)
                {
                    if(strcmp(optarg, charstyle_str[i][0]) == 0)
                    {
                        charstyle = i;
                    }
                }
                if(charstyle == CHARSTYLE_MAX) // No valid value found?
                {
                    printf("Invalid charstyle value: %s\n", optarg);
                    printf("Charstyle must be one of:");
                    for(int i=0; i<CHARSTYLE_MAX; i++)
                        printf(" %s", charstyle_str[i][0]);
                    printf("\n");
                    exit(1);
                }
                break;
            }

            case 'h':
            {
                printf("Usage:\n");
                printf("  %s [options]\n", SW_NAME);
                printf("\n");
                printf("%s - ncurses Game of Life %s (compiled %s %s) by %s\n", SW_NAME, SW_VERS, __DATE__, __TIME__, AUTHOR_LONG);
                printf("\n");
                printf("Options:\n");
                printf("  -c, --charstyle  Set character style:\n");
                for(int i=0; i<CHARSTYLE_MAX; i++)
                    printf("                   - %-7s -> %s\n", charstyle_str[i][0], charstyle_str[i][1]);
                printf("  -h, --help       This Help\n");
                printf("  -i, --init       Set initial pattern:\n");
                for(int i=0; i<INITPATTERN_CYCLEMAX; i++)
                    printf("                   - %-9s -> %s\n", grid_get_initpattern_short_str(i), grid_get_initpattern_long_str(i));
                printf("  -m, --mode       Set mode:\n");
                for(int i=0; i<MODE_MAX; i++)
                    printf("                   - %-4s -> %s\n", automode_str[i][0], automode_str[i][1]);
                printf("  -n, --nowait     Start without Startupscreen\n");
                printf("  -s, --speed      Set speed (0-9)\n");
                printf("\n");
                printf(COMMAND_KEYS_STR);
                exit(0);
            }

            case 'i':
            {
                initpattern = INITPATTERN_MAX;
                for(int i=0; i<INITPATTERN_CYCLEMAX; i++)
                {
                    if(strcmp(optarg, grid_get_initpattern_short_str(i)) == 0)
                    {
                        initpattern = i;
                    }
                }
                if(initpattern == INITPATTERN_MAX) // No valid value found?
                {
                    printf("Invalid init value: %s\n", optarg);
                    printf("Init must be one of:");
                    for(int i=0; i<INITPATTERN_CYCLEMAX; i++)
                        printf(" %s", grid_get_initpattern_short_str(i));
                    printf("\n");
                    exit(1);
                }
                break;
            }

            case 'm':
            {
                automode = MODE_MAX;
                for(int i=0; i<MODE_MAX; i++)
                {
                    if(strcmp(optarg, automode_str[i][0]) == 0)
                    {
                        automode = i;
                    }
                }
                if(automode == MODE_MAX) // No valid value found?
                {
                    printf("Invalid mode value: %s\n", optarg);
                    printf("Mode must be one of:");
                    for(int i=0; i<MODE_MAX; i++)
                        printf(" %s", automode_str[i][0]);
                    printf("\n");
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
                printf("%s - ncurses Game of Life %s (compiled %s %s) by %s\n", SW_NAME, SW_VERS, __DATE__, __TIME__, AUTHOR_LONG);
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
