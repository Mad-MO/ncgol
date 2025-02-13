
// File:    ncgol.c
// Author:  Martin Ochs
// License: MIT
// Brief:   A very basic python implementation for Conway's "Game of Life"
//          using ncurses for the GUI. The simulation can be controlled by
//          the arrow keys and the buttons for different initial states.
//
// Rules:   https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life
// Ncurses: https://tldp.org/HOWTO/NCURSES-Programming-HOWTO/
// Unicode: https://www.compart.com/en/unicode/block/U+2580
//          https://www.compart.com/en/unicode/block/U+2800

// TODO: Commandline options
// TODO: Codecosmetic / Indentation
// TODO: Improve the end detection (Large grid with gliders)
// TODO: Rename "mode" -> "pattern"
// TODO: Mode: once / loop / next
// TODO: Detect terminal type and set best style

#include <curses.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <locale.h>
#include <ctype.h>
#include <time.h>



// Define SW name and Version
#define SW_NAME "ncgol"
#define SW_VERS "v0.4"
#define AUTHOR  "by domo"

// Define the size of the grid
// Example: Fullscreen Terminal on Ultrawidescreen Monitor has ~569x110 characters
//          -> 1134x420 cells when using braille style
#define GRID_WIDTH_MAX  2500 // 2500*1000*2 = ~5 MB
#define GRID_HEIGHT_MAX 1000

// Create the grid to represent the cells
static uint8_t grid[GRID_WIDTH_MAX][GRID_HEIGHT_MAX];
static uint8_t new_grid[GRID_WIDTH_MAX][GRID_HEIGHT_MAX];
static uint8_t speed;
static uint16_t cells_alive = 0;
static uint32_t cycle_counter = 0;
#define END_DET_CNT 60
static uint8_t end_det[END_DET_CNT];
static uint8_t end_det_pos;
static uint16_t grid_width;
static uint16_t grid_height;

WINDOW *w_grid_box;
WINDOW *w_grid;
WINDOW *w_status_box;
WINDOW *w_status;

typedef enum
{
    ModeTypeRandom,
    ModeTypeBlinker,
    ModeTypeGlider,
    ModeTypeGliderGun,
    ModeTypePentomino,
    ModeTypeDiehard,
    ModeTypeAcorn,
    // ----------------
    ModeTypeMax
} ModeType;
static ModeType mode;

typedef enum
{
    StageTypeStartup,  // Show startup screen
    StageTypeInit,     // Initialize grid
    StageTypeShowInfo, // Show info for current mode
    StageTypeRunning,  // Running simulation
    StageTypeEnd       // End of simulation has been reached
} StageType;
static StageType stage;

typedef enum
{
    // Styles which use two chars per cell
    StyleTypeUnicodeBlock1,
    StyleTypeUnicodeBlock2,
    StyleTypeASCIIblockSquare,
    StyleTypeASCIIhash,

    // Styles which show two cells in one char
    StyleTypeUnicodeDoubleBlock,
    StyleTypeUnicodeDoubleDots,
    StyleTypeASCIIdouble,

    // Braile style with 8 dots per char
    StyleTypeUnicodeBraille,
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
    COLOR_PAIR_MODE_INFO
} ColorPairType;
static ColorPairType color_pair;


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
  init_pair(COLOR_PAIR_STANDARD,  COLOR_WHITE,   COLOR_BLACK);   // Standard
  init_pair(COLOR_PAIR_LABEL,     COLOR_GREEN,   COLOR_BLACK);   // Label
  init_pair(COLOR_PAIR_VALUE,     COLOR_YELLOW,  COLOR_BLACK);   // Value
  init_pair(COLOR_PAIR_LIVE_CELL, COLOR_WHITE,   COLOR_BLACK);   // Live cell
  init_pair(COLOR_PAIR_TITLE,     COLOR_CYAN,    COLOR_BLACK);   // Title
  init_pair(COLOR_PAIR_MODE_INFO, COLOR_WHITE,   COLOR_MAGENTA); // Mode info
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
  memset(new_grid, 0, sizeof(new_grid));

  if (style == StyleTypeUnicodeBraille)
  {
    grid_width  = getmaxx(w_grid) * 2;
    grid_height = getmaxy(w_grid) * 4;
  }
  else if(style >= StyleTypeUnicodeDoubleBlock)
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
}



// Function to initialize the grid
void init_grid(void)
{
    memset(grid, 0, sizeof(grid));

    if(mode == ModeTypeRandom)
    {
        uint16_t x, y;
        for(x=0; x<grid_width; x++)
            for(y=0; y<grid_height; y++)
                grid[x][y] = (random() & 1);
    }
    else if(mode == ModeTypeBlinker)
    {
        grid[1+(grid_width/2)][0+(grid_height/2)] = 1;
        grid[1+(grid_width/2)][1+(grid_height/2)] = 1;
        grid[1+(grid_width/2)][2+(grid_height/2)] = 1;
    }
    else if(mode == ModeTypeGlider)
    {
        grid[0+(grid_width/2)][2+(grid_height/2)] = 1;
        grid[1+(grid_width/2)][0+(grid_height/2)] = 1;
        grid[1+(grid_width/2)][2+(grid_height/2)] = 1;
        grid[2+(grid_width/2)][1+(grid_height/2)] = 1;
        grid[2+(grid_width/2)][2+(grid_height/2)] = 1;
    }
    else if(mode == ModeTypeGliderGun)
    {
        grid[ 1][5] = 1; grid[ 1][6] = 1;
        grid[ 2][5] = 1; grid[ 2][6] = 1;
        grid[11][5] = 1; grid[11][6] = 1; grid[11][7] = 1;
        grid[12][4] = 1; grid[12][8] = 1;
        grid[13][3] = 1; grid[13][9] = 1;
        grid[14][3] = 1; grid[14][9] = 1;
        grid[15][6] = 1;
        grid[16][4] = 1; grid[16][8] = 1;
        grid[17][5] = 1; grid[17][6] = 1; grid[17][7] = 1;
        grid[18][6] = 1;
        grid[21][3] = 1; grid[21][4] = 1; grid[21][5] = 1;
        grid[22][3] = 1; grid[22][4] = 1; grid[22][5] = 1;
        grid[23][2] = 1; grid[23][6] = 1;
        grid[25][1] = 1; grid[25][2] = 1; grid[25][6] = 1; grid[25][7] = 1;
        grid[35][3] = 1; grid[35][4] = 1;
        grid[36][3] = 1; grid[36][4] = 1;
    }
    else if(mode == ModeTypePentomino)
    {
        grid[0+(grid_width/2)][1+(grid_height/2)] = 1;
        grid[1+(grid_width/2)][0+(grid_height/2)] = 1;
        grid[1+(grid_width/2)][1+(grid_height/2)] = 1;
        grid[1+(grid_width/2)][2+(grid_height/2)] = 1;
        grid[2+(grid_width/2)][0+(grid_height/2)] = 1;
    }
    else if(mode == ModeTypeDiehard)
    {
        grid[0+(grid_width/2)][4+(grid_height/2)] = 1;
        grid[1+(grid_width/2)][4+(grid_height/2)] = 1;
        grid[1+(grid_width/2)][5+(grid_height/2)] = 1;
        grid[5+(grid_width/2)][5+(grid_height/2)] = 1;
        grid[6+(grid_width/2)][3+(grid_height/2)] = 1;
        grid[6+(grid_width/2)][5+(grid_height/2)] = 1;
        grid[7+(grid_width/2)][5+(grid_height/2)] = 1;
    }
    else if(mode == ModeTypeAcorn)
    {
        grid[0+(grid_width/2)][4+(grid_height/2)] = 1;
        grid[1+(grid_width/2)][2+(grid_height/2)] = 1;
        grid[1+(grid_width/2)][4+(grid_height/2)] = 1;
        grid[3+(grid_width/2)][3+(grid_height/2)] = 1;
        grid[4+(grid_width/2)][4+(grid_height/2)] = 1;
        grid[5+(grid_width/2)][4+(grid_height/2)] = 1;
        grid[6+(grid_width/2)][4+(grid_height/2)] = 1;
    }

    cycle_counter = 0;
}



// Function to update the grid based on the game of life rules
void update_grid(void)
{
    uint16_t x, y;

    for(x=0; x<grid_width; x++)
    {
        for(y=0; y<grid_height; y++)
        {
            uint8_t neighbors = 0;
            for(int8_t dx=-1; dx<=1; dx++)
            {
                for(int8_t dy=-1; dy<=1; dy++)
                {
                    if(dx == 0 && dy == 0)
                    {
                        continue;
                    }
                    uint16_t nx = (grid_width + x + dx) % grid_width;
                    uint16_t ny = (grid_height + y + dy) % grid_height;
                    neighbors += grid[nx][ny];
                }
            }

            if     ((grid[x][y] == 1) && (neighbors  < 2)) // Underpopulation
                new_grid[x][y] = 0;
            else if((grid[x][y] == 1) && (neighbors  > 3)) // Overpopulation
                new_grid[x][y] = 0;
            else if((grid[x][y] == 0) && (neighbors == 3)) // Reproduction
                new_grid[x][y] = 1;
            else                                           // Stasis
                new_grid[x][y] = grid[x][y];
        }
    }

    memcpy(grid, new_grid, sizeof(grid));
}



// Function to draw a string in a rounded frame for the mode name
static void draw_str_in_frame(const char * str)
{
    int len=strlen(str);
    int x = (getmaxx(w_grid) - len) / 2 - 2;
    int y = (getmaxy(w_grid)) / 4;

    wattron(w_grid, A_BOLD | COLOR_PAIR(COLOR_PAIR_MODE_INFO));
    for(int i=0; i<len+4; i++)
    {
        mvwaddch(w_grid, y+0, x+i, ' ');
        mvwaddch(w_grid, y+1, x+i, ' ');
        mvwaddch(w_grid, y+2, x+i, ' ');
    }
    mvwaddstr(w_grid, y+1, x+2, str);
    wattroff(w_grid, A_BOLD | COLOR_PAIR(COLOR_PAIR_MODE_INFO));
}



// Function to draw the grid on the canvas
static void draw_grid(void)
{
    uint16_t x, y;
    char str[16];

    cells_alive = 0;

    // Draw grid to canvas
    wattron(w_grid, A_BOLD | COLOR_PAIR(COLOR_PAIR_LIVE_CELL));
    for(x=0; x<grid_width; x++)
    {
        for(y=0; y<grid_height; y++)
        {
            if ((style == StyleTypeUnicodeDoubleBlock) || (style == StyleTypeUnicodeDoubleDots) || (style == StyleTypeASCIIdouble))
            {
                // Two dots per character
                if (grid[x][y] && grid[x][y + 1])
                {
                    if (style == StyleTypeUnicodeDoubleBlock)
                        mvwaddstr(w_grid, y/2, x, "\u2588");
                    else if (style == StyleTypeUnicodeDoubleDots)
                        mvwaddstr(w_grid, y/2, x, "\u2805");
                    else // StyleTypeASCIIdouble
                        mvwaddch(w_grid, y/2, x, ':');
                    cells_alive += 2;
                }
                else if (grid[x][y])
                {
                    if (style == StyleTypeUnicodeDoubleBlock)
                        mvwaddstr(w_grid, y/2, x, "\u2580");
                    else if (style == StyleTypeUnicodeDoubleDots)
                        mvwaddstr(w_grid, y/2, x, "\u2801");
                    else // StyleTypeASCIIdouble
                        mvwaddch(w_grid, y/2, x, '\'');
                    cells_alive++;
                }
                else if (grid[x][y + 1])
                {
                    if (style == StyleTypeUnicodeDoubleBlock)
                        mvwaddstr(w_grid, y/2, x, "\u2584");
                    else if (style == StyleTypeUnicodeDoubleDots)
                        mvwaddstr(w_grid, y/2, x, "\u2804");
                    else // StyleTypeASCIIdouble
                        mvwaddch(w_grid, y/2, x, '.');
                    cells_alive++;
                }
                else
                {
                    mvwaddch(w_grid, y/2, x, ' ');
                }
                y++;
            }
            else if (style == StyleTypeUnicodeBraille)
            {
                // The braille pattern allows the usage of 8 dots per character
                uint16_t braille = 0;
                wchar_t braille_char;
                char braille_str[4] = {0};

                // Convert braille pattern to unicode character
                if (grid[x+0][y+0]) {braille |= 0x01; cells_alive++;}
                if (grid[x+0][y+1]) {braille |= 0x02; cells_alive++;}
                if (grid[x+0][y+2]) {braille |= 0x04; cells_alive++;}
                if (grid[x+0][y+3]) {braille |= 0x40; cells_alive++;}
                if (grid[x+1][y+0]) {braille |= 0x08; cells_alive++;}
                if (grid[x+1][y+1]) {braille |= 0x10; cells_alive++;}
                if (grid[x+1][y+2]) {braille |= 0x20; cells_alive++;}
                if (grid[x+1][y+3]) {braille |= 0x80; cells_alive++;}
                braille |= 0x2800;
                braille_char = braille;
                wcstombs(braille_str, &braille_char, 4);

                mvwaddstr(w_grid, y/4, x/2, braille_str);

                y+=3;
                if(y >= grid_height)
                  x+=1;
            }
            else
            {
                // Two characters represent one cell
                // Using background color with an empy space works not very well in ncurses,
                // because the background color is only dimmed and not bright.
                // A unicode full block uses the foreground color and works better.
                if (grid[x][y])
                {
                    if (style == StyleTypeUnicodeBlock1)
                        mvwaddstr(w_grid, y, x*2, "\u2588\u2588"); // Two full blocks -> Looks best on a linux terminal which leaves no horizontal space between the blocks
                    else if (style == StyleTypeUnicodeBlock2)
                        mvwaddstr(w_grid, y, x*2, "\u2588\u258a"); // Full block and 3/4 block -> Looks best on a mac terminal which leaves a little horizontal space between the blocks
                    else if (style == StyleTypeASCIIhash)
                        mvwaddstr(w_grid, y, x*2, "# ");           // #  -> Looks best on a terminal which has problems with unicode characters
                    else
                        mvwaddstr(w_grid, y, x*2, "[]");           // [] -> Looks best on a terminal which has problems with unicode characters
                    cells_alive++;
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
    if(stage == StageTypeStartup)
    {
        wmove(w_grid, 0, 0);
        waddstr(w_grid, " \n");
        wattron(w_grid, A_BOLD | A_REVERSE);
        waddstr(w_grid, " A ncurses based Simulation for Conways \"Game of Life\" \n");
        wattroff(w_grid, A_BOLD | A_REVERSE);
        waddstr(w_grid, " \n");
        waddstr(w_grid, " Basic command keys:\n");
        waddstr(w_grid, "   \'q\'                 End program\n");
        waddstr(w_grid, "   \'0\'...\'9\'           Set speed directly\n");
        waddstr(w_grid, "   \'Up\' and \'Down\'     Adjust speed\n");
        waddstr(w_grid, "   \'Left\' and \'Right\'  Change mode\n");
        waddstr(w_grid, "   \'Space\'             Restart current mode\n");
        waddstr(w_grid, "   \'s\'                 Change style\n");
        waddstr(w_grid, "   \'h\'                 This Help\n");
        waddstr(w_grid, " \n");
        waddstr(w_grid, " Mode keys:\n");
        waddstr(w_grid, "   \'r\'                 Random\n");
        waddstr(w_grid, "   \'b\'                 Blinker\n");
        waddstr(w_grid, "   \'g\'                 Glider\n");
        waddstr(w_grid, "   \'l\'                 Glider gun\n");
        waddstr(w_grid, "   \'p\'                 Pentomino\n");
        waddstr(w_grid, "   \'d\'                 Diehard\n");
        waddstr(w_grid, "   \'a\'                 Acorn\n");
    }

    // Handle mode info
    if(stage == StageTypeShowInfo)
    {
        if     (mode == ModeTypeRandom)
            draw_str_in_frame("Random");
        else if(mode == ModeTypeBlinker)
            draw_str_in_frame("Blinker");
        else if(mode == ModeTypeGlider)
            draw_str_in_frame("Glider");
        else if(mode == ModeTypeGliderGun)
            draw_str_in_frame("Glider gun");
        else if(mode == ModeTypePentomino)
            draw_str_in_frame("Pentomino");
        else if(mode == ModeTypeDiehard)
            draw_str_in_frame("Diehard");
        else if(mode == ModeTypeAcorn)
            draw_str_in_frame("Acorn");
    }

    // Handle status line
    {
        // Full line ==> "Grid:2500x1000 Cycles:123456 Cells:1500000 Speed:10 Mode:Glider gun Style:DoubleBlock   ncgol v0.x by domo"
        char str_label[20];
        char str_value[20];
        uint16_t pos = 0;
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
        sprintf(str_value, "%3u", cycle_counter);
        if((getcurx(w_status)+strlen(str_label)+strlen(str_value)) < width)
        {
            wattron(w_status, COLOR_PAIR(COLOR_PAIR_LABEL));
            waddstr(w_status, str_label);
            wattron(w_status, COLOR_PAIR(COLOR_PAIR_VALUE));
            waddstr(w_status, str_value);
        }

        // Cells
        strcpy(str_label, " Cells:");
        sprintf(str_value, "%3u", cells_alive);
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

        // Mode
        strcpy(str_label, " Mode:");
        if     (mode == ModeTypeRandom)
            strcpy(str_value, "Random");
        else if(mode == ModeTypeBlinker)
            strcpy(str_value, "Blinker");
        else if(mode == ModeTypeGlider)
            strcpy(str_value, "Glider");
        else if(mode == ModeTypeGliderGun)
            strcpy(str_value, "Glider gun");
        else if(mode == ModeTypePentomino)
            strcpy(str_value, "Pentomino");
        else if(mode == ModeTypeDiehard)
            strcpy(str_value, "Diehard");
        else if(mode == ModeTypeAcorn)
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
        if     (style == StyleTypeUnicodeBlock1)
            strcpy(str_value, "Block1");
        else if(style == StyleTypeUnicodeBlock2)
            strcpy(str_value, "Block2");
        else if(style == StyleTypeASCIIblockSquare)
            strcpy(str_value, "SquareASCII");
        else if(style == StyleTypeASCIIhash)
            strcpy(str_value, "#ASCII");
        else if(style == StyleTypeUnicodeDoubleBlock)
            strcpy(str_value, "DoubleBlock");
        else if(style == StyleTypeUnicodeDoubleDots)
            strcpy(str_value, "DoubleDots");
        else if(style == StyleTypeASCIIdouble)
            strcpy(str_value, "DoubleASCII");
        else if(style == StyleTypeUnicodeBraille)
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

        // Remember position and clear rest of line
        waddstr(w_status, "   ");
        pos = getcurx(w_status);
        for(int i=pos; i<width; i++)
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
        if(stage == StageTypeStartup)
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
      mode++;
      mode %= ModeTypeMax;
      stage = StageTypeInit;
    }
    else if(key == KEY_LEFT)
    {
      if(mode == 0) mode = ModeTypeMax - 1;
      else          mode--;
      stage = StageTypeInit;
    }
    else if(tolower(key) == 's')
    {
        style++;
        style %= StyleTypeMax;
        init_tui();
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
    else if(tolower(key) == 'r')
    {
        mode = ModeTypeRandom;
        stage = StageTypeInit;
    }
    else if(key == ' ')
    {
        stage = StageTypeInit;
    }
    else if(tolower(key) == 'b')
    {
      mode = ModeTypeBlinker;
      stage = StageTypeInit;
    }
    else if(tolower(key) == 'g')
    {
      mode = ModeTypeGlider;
      stage = StageTypeInit;
    }
    else if(tolower(key) == 'l')
    {
      mode = ModeTypeGliderGun;
      stage = StageTypeInit;
    }
    else if(tolower(key) == 'p')
    {
      mode = ModeTypePentomino;
      stage = StageTypeInit;
    }
    else if(tolower(key) == 'd')
    {
      mode = ModeTypeDiehard;
      stage = StageTypeInit;
    }
    else if(tolower(key) == 'a')
    {
      mode = ModeTypeAcorn;
      stage = StageTypeInit;
    }
    else if(tolower(key) == 'h')
    {
      stage = StageTypeStartup;
      memset(grid, 0, sizeof(grid));
    }
    else
    {
        // Do nothing
    }
}



uint8_t end_detection(void)
{
    end_det_pos++;
    end_det_pos %= END_DET_CNT;
    end_det[end_det_pos] = cells_alive;
    if(cells_alive == 0)
        return 1;
    if(cycle_counter > END_DET_CNT)                                 // At least END_DET_CNT cycles needed for detection
    {
        for(uint8_t pattern=1; pattern<=(END_DET_CNT/2); pattern++) // Test pattern in the length of 1 to half of the buffer
        {
            for(uint8_t testpos=pattern; testpos<END_DET_CNT; testpos++)
            {
                if(end_det[testpos] != end_det[testpos % pattern])  // Pattern not found? -> End loop and test next pattern
                    break;
                if(testpos == END_DET_CNT - 1)                      // End of loop reached? -> Pattern found!
                    return 1;
            }
        }
    }
    return 0;
}



// Function to handle one life cycle of the simulation
int main(void)
{
  // Initialize variables
  mode  = ModeTypeRandom;
  speed = 3;
  style = StyleTypeUnicodeBlock2;

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

    // Handle different modes and update grid
    if     (stage == StageTypeStartup)
    {
        if(timer >= 20000)
        {
            stage = StageTypeInit;
            timer = 0;
        }
    }
    else if(stage == StageTypeInit)
    {
        init_grid();
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
            cycle_counter++;
            update_grid();
        }
    }
    else if(stage == StageTypeEnd)
    {
        update_grid();
        if(timer >= 5000)
        {
            mode++;
            mode %= ModeTypeMax;
            stage = StageTypeInit;
            timer = 0;
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
