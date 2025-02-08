
// https://tldp.org/HOWTO/NCURSES-Programming-HOWTO/

#include <curses.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>



// Define the size of the grid
#define GRID_WIDTH    200
#define GRID_HEIGHT   100

// Define the size of the grid
static uint8_t grid[GRID_WIDTH][GRID_HEIGHT];
static uint8_t new_grid[GRID_WIDTH][GRID_HEIGHT];
int speed = 5;
int cells_alive = 0;
int cycle_counter = 0;
#define END_DET_CNT 60
static uint8_t end_det[END_DET_CNT];
static uint8_t end_det_pos;

WINDOW *w_grid_box;
WINDOW *w_grid;
WINDOW *w_status_box;
WINDOW *w_status;



// Function to initialize the grid
void init_grid(void)
{
    memset(grid, 0, sizeof(grid));

    {
        uint8_t x, y;
        for(x=0; x<GRID_WIDTH; x++)
            for(y=0; y<GRID_HEIGHT; y++)
                grid[x][y] = (random() & 1);
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

    // Clear display
    //wclear(w_grid);
    cells_alive = 0;

    // Draw grid to canvas
    for(x=0; x<GRID_WIDTH; x++)
    {
        for(y=0; y<GRID_HEIGHT; y++)
        {
            if(grid[x][y])
            {
              mvwaddch(w_grid, y, x, '#');
              cells_alive++;
            }
            else
            {
              mvwaddch(w_grid, y, x, ' ');
            }
        }
    }

    // Handle status line
    wattron(w_status, COLOR_PAIR(2));
    mvwprintw(w_status, 0, 0, "Cycles =");
    wattron(w_status, COLOR_PAIR(3));
    mvwprintw(w_status, 0, 9, "%d", cycle_counter);

    wattron(w_status, COLOR_PAIR(2));
    mvwprintw(w_status, 0, 20, "Cells =");
    wattron(w_status, COLOR_PAIR(3));
    mvwprintw(w_status, 0, 28, "%d", cells_alive);

    wattron(w_status, COLOR_PAIR(2));
    mvwprintw(w_status, 0, 40, "Speed =");
    wattron(w_status, COLOR_PAIR(3));
    mvwprintw(w_status, 0, 48, "%d", speed);

    wattroff(w_status, COLOR_PAIR(3));
    wrefresh(w_status);
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
  clear();   // Clear the screen

  // Prepare coloring
  start_color();
  init_pair(1, COLOR_WHITE, COLOR_BLACK);
  init_pair(2, COLOR_RED,   COLOR_BLACK);
  init_pair(3, COLOR_WHITE,  COLOR_BLUE);
  ESCDELAY = 1; // Set the delay for escape sequences

  // Create status window
  w_status_box = newwin(3, COLS, LINES-3, 0);
  box(w_status_box, 0, 0); // Draw a box around the screen
  wrefresh(w_status_box);
  w_status = newwin(1, COLS-2, LINES-2, 1);
  wrefresh(w_status);
  //nodelay(status, TRUE); // Non-blocking input
  keypad(w_status, TRUE); // Enable special keys

  // Create grid window
  w_grid_box = newwin(LINES-3, COLS, 0, 0);
  box(w_grid_box, 0, 0); // Draw a box around the screen
  wrefresh(w_grid_box);
  w_grid = newwin(LINES-5, COLS-2, 1, 1);
  wrefresh(w_grid);
  nodelay(w_grid, TRUE); // Non-blocking input
  keypad(w_grid, TRUE); // Enable special keys

  // Do something
  init_grid();
  draw_grid();
  wrefresh(w_grid);

  while(1)
  {
    // Handle one cycle
    usleep(10000);
    cycle_counter++;
    update_grid();
    draw_grid();
    wrefresh(w_grid);

    // Handle exit
    int z = wgetch(w_grid);
    if(z == 27)
    {
      endwin();
      exit(0);
    }
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
