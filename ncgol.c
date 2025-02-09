
// File:    ncgol.c
// Author:  Martin Ochs
// License: MIT
// Brief:   A very basic python implementation for Conway's "Game of Life"
//          using ncurses for the GUI. The simulation can be controlled by
//          the arrow keys and the buttons for different initial states.

// Ncurses: https://tldp.org/HOWTO/NCURSES-Programming-HOWTO/



#include <curses.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


// Define SW name and Version
#define SW_NAME "ncgol"
#define SW_VERS "v0.1"
#define AUTHOR  "by domo"
#define SW_STR  SW_NAME " " SW_VERS " " AUTHOR

// Define the size of the grid
#define GRID_WIDTH    100
#define GRID_HEIGHT   100

// Create the grid to represent the cells
static uint8_t grid[GRID_WIDTH][GRID_HEIGHT];
static uint8_t new_grid[GRID_WIDTH][GRID_HEIGHT];
static uint8_t speed;
int cells_alive = 0;
int cycle_counter = 0;
#define END_DET_CNT 60
static uint8_t end_det[END_DET_CNT];
static uint8_t end_det_pos;

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




// Function to initialize the grid
void init_grid(void)
{
    memset(grid, 0, sizeof(grid));

    if(mode == ModeTypeRandom)
    {
        uint8_t x, y;
        for(x=0; x<GRID_WIDTH; x++)
            for(y=0; y<GRID_HEIGHT; y++)
                grid[x][y] = (random() & 1);
    }
    else if(mode == ModeTypeBlinker)
    {
        grid[1+(GRID_WIDTH/2)][0+(GRID_HEIGHT/2)] = 1;
        grid[1+(GRID_WIDTH/2)][1+(GRID_HEIGHT/2)] = 1;
        grid[1+(GRID_WIDTH/2)][2+(GRID_HEIGHT/2)] = 1;
    }
    else if(mode == ModeTypeGlider)
    {
        grid[0+(GRID_WIDTH/2)][2+(GRID_HEIGHT/2)] = 1;
        grid[1+(GRID_WIDTH/2)][0+(GRID_HEIGHT/2)] = 1;
        grid[1+(GRID_WIDTH/2)][2+(GRID_HEIGHT/2)] = 1;
        grid[2+(GRID_WIDTH/2)][1+(GRID_HEIGHT/2)] = 1;
        grid[2+(GRID_WIDTH/2)][2+(GRID_HEIGHT/2)] = 1;
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
        grid[0+(GRID_WIDTH/2)][1+(GRID_HEIGHT/2)] = 1;
        grid[1+(GRID_WIDTH/2)][0+(GRID_HEIGHT/2)] = 1;
        grid[1+(GRID_WIDTH/2)][1+(GRID_HEIGHT/2)] = 1;
        grid[1+(GRID_WIDTH/2)][2+(GRID_HEIGHT/2)] = 1;
        grid[2+(GRID_WIDTH/2)][0+(GRID_HEIGHT/2)] = 1;
    }
    else if(mode == ModeTypeDiehard)
    {
        grid[0+(GRID_WIDTH/2)][4+(GRID_HEIGHT/2)] = 1;
        grid[1+(GRID_WIDTH/2)][4+(GRID_HEIGHT/2)] = 1;
        grid[1+(GRID_WIDTH/2)][5+(GRID_HEIGHT/2)] = 1;
        grid[5+(GRID_WIDTH/2)][5+(GRID_HEIGHT/2)] = 1;
        grid[6+(GRID_WIDTH/2)][3+(GRID_HEIGHT/2)] = 1;
        grid[6+(GRID_WIDTH/2)][5+(GRID_HEIGHT/2)] = 1;
        grid[7+(GRID_WIDTH/2)][5+(GRID_HEIGHT/2)] = 1;
    }
    else if(mode == ModeTypeAcorn)
    {
        grid[0+(GRID_WIDTH/2)][4+(GRID_HEIGHT/2)] = 1;
        grid[1+(GRID_WIDTH/2)][2+(GRID_HEIGHT/2)] = 1;
        grid[1+(GRID_WIDTH/2)][4+(GRID_HEIGHT/2)] = 1;
        grid[3+(GRID_WIDTH/2)][3+(GRID_HEIGHT/2)] = 1;
        grid[4+(GRID_WIDTH/2)][4+(GRID_HEIGHT/2)] = 1;
        grid[5+(GRID_WIDTH/2)][4+(GRID_HEIGHT/2)] = 1;
        grid[6+(GRID_WIDTH/2)][4+(GRID_HEIGHT/2)] = 1;
    }

    cycle_counter = 0;
}



// Function to update the grid based on the game of life rules
void update_grid(void)
{
    uint8_t x, y;
    for(x=0; x<GRID_WIDTH; x++)
    {
        for(y=0; y<GRID_HEIGHT; y++)
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
                    uint8_t nx = (GRID_WIDTH + x + dx) % GRID_WIDTH;
                    uint8_t ny = (GRID_HEIGHT + y + dy) % GRID_HEIGHT;
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



// Function to draw the grid on the canvas
static void draw_grid(void)
{
    uint8_t x, y;
    char str[16];

    cells_alive = 0;

    // Draw grid to canvas
    for(x=0; x<GRID_WIDTH; x++)
    {
        for(y=0; y<GRID_HEIGHT; y++)
        {
            if(grid[x][y])
            {
              wattron(w_grid, COLOR_PAIR(4));
              mvwaddch(w_grid, y, x*2+0, '[');
              mvwaddch(w_grid, y, x*2+1, ']');
              cells_alive++;
            }
            else
            {
              wattron(w_grid, COLOR_PAIR(5));
              mvwaddch(w_grid, y, x*2+0, ' ');
              mvwaddch(w_grid, y, x*2+1, ' ');
            }
      }
    }
    wattroff(w_status, COLOR_PAIR(1));

    // Handle mode info
    wattron(w_status, COLOR_PAIR(2));
    mvwprintw(w_status, 0, 40, "Mode:          ");
    wattron(w_status, COLOR_PAIR(3));
    if     (mode == ModeTypeRandom)
        mvwaddstr(w_status, 0, 45, "Random");
    else if(mode == ModeTypeBlinker)
        mvwaddstr(w_status, 0, 45, "Blinker");
    else if(mode == ModeTypeGlider)
        mvwaddstr(w_status, 0, 45, "Glider");
    else if(mode == ModeTypeGliderGun)
        mvwaddstr(w_status, 0, 45, "Glider gun");
    else if(mode == ModeTypePentomino)
        mvwaddstr(w_status, 0, 45, "Pentomino");
    else if(mode == ModeTypeDiehard)
        mvwaddstr(w_status, 0, 45, "Diehard");
    else if(mode == ModeTypeAcorn)
        mvwaddstr(w_status, 0, 45, "Acorn");

    // Handle status line
    wattron(w_status, COLOR_PAIR(2));
    mvwprintw(w_status, 0, 0, "Cycles:       ");
    wattron(w_status, COLOR_PAIR(3));
    mvwprintw(w_status, 0, 7, "%d", cycle_counter);

    wattron(w_status, COLOR_PAIR(2));
    mvwprintw(w_status, 0, 16, "Cells:      ");
    wattron(w_status, COLOR_PAIR(3));
    mvwprintw(w_status, 0, 22, "%d", cells_alive);

    wattron(w_status, COLOR_PAIR(2));
    mvwprintw(w_status, 0, 30, "Speed:  ");
    wattron(w_status, COLOR_PAIR(3));
    mvwprintw(w_status, 0, 36, "%d", speed);

    wattroff(w_status, COLOR_PAIR(0));
    wrefresh(w_status);
}



// Function to handle input events
void handle_inputs(void)
{
    int z = wgetch(w_grid);
    if((z=='q') || (z==27)) // 'q' or ESC
    {
      endwin();
      exit(0);
    }
    else if(z == KEY_UP)
    {
      if(speed < 10) speed++;
    }
    else if(z == KEY_DOWN)
    {
      if(speed > 1) speed--;
    }
    else if(z == KEY_RIGHT)
    {
      mode++;
      mode %= ModeTypeMax;

      init_grid();
      draw_grid();
      wrefresh(w_grid);
    }
    else if(z == KEY_LEFT)
    {
      if(mode == 0) mode = ModeTypeMax - 1;
      else          mode--;

      init_grid();
      draw_grid();
      wrefresh(w_grid);
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
  // Initialize ncurses
  initscr(); // Determine terminal type
  cbreak();  // Disable buffering
  noecho();  // Disable echo to the screen
  curs_set(0); // Hide the cursor
  clear();   // Clear the screen

  // Prepare coloring
  start_color();
  init_pair(1, COLOR_WHITE,  COLOR_BLACK); // Standard
  init_pair(2, COLOR_GREEN,  COLOR_BLACK); // Label
  init_pair(3, COLOR_YELLOW, COLOR_BLACK); // Value
  init_pair(4, COLOR_BLACK,  COLOR_RED);   // Live cell
  init_pair(5, COLOR_BLACK,  COLOR_BLACK); // Dead cell
  init_pair(6, COLOR_CYAN,   COLOR_BLACK); // Title
  ESCDELAY = 1; // Set the delay for escape sequences

  // Create status window
  w_status_box = newwin(3, COLS, LINES-3, 0);
  box(w_status_box, 0, 0); // Draw a box around the screen
  wattron(w_status_box, COLOR_PAIR(6));
  mvwaddstr(w_status_box, 0, 3, " Status ");
  wattroff(w_status_box, COLOR_PAIR(0));
  wrefresh(w_status_box);
  w_status = newwin(1, COLS-2, LINES-2, 1);
  mvwaddstr(w_status, 0, getmaxx(w_status)-strlen(SW_STR), SW_STR); // Todo: Subtract from real width
  wrefresh(w_status);
  keypad(w_status, TRUE); // Enable special keys

  // Create grid window
  w_grid_box = newwin(LINES-3, COLS, 0, 0);
  box(w_grid_box, 0, 0); // Draw a box around the screen
  wattron(w_grid_box, COLOR_PAIR(6));
  mvwaddstr(w_grid_box, 0, 3, " Game of Life ");
  wattroff(w_grid_box, COLOR_PAIR(0));
  wrefresh(w_grid_box);
  w_grid = newwin(LINES-5, COLS-2, 1, 1);
  wrefresh(w_grid);
  nodelay(w_grid, TRUE); // Non-blocking input
  keypad(w_grid, TRUE); // Enable special keys

  // Init
  mode  = ModeTypeRandom;
  speed = 4;

  init_grid();
  draw_grid();
  wrefresh(w_grid);

  // Loop until back key is pressed
  while(1)
  {
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

    // Handle one cycle
    if(end_detection())
    {
    }
    else
    {
      cycle_counter++;
    }
    update_grid();
    draw_grid();
    wrefresh(w_grid);

    // Handle input
    handle_inputs();
  }

  // Ask to end program
  wattron(w_status, COLOR_PAIR(1));
  mvwaddstr(w_status, 0, 0, "Press any key to quit");
  wattroff(w_status, COLOR_PAIR(1));
  wrefresh(w_status);
  wgetch(w_status);

  // End program
  endwin();
  exit(0);
}
