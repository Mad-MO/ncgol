
# Ncurses Game of Life

TODO: Currently in Development. Might still contain a lot of bugs!

Based on Ncurses.

## Description

A basic implementation for Conway's "Game of Life" in Ncurses.

## Screenshot

![Screenshot](./doc/Screenshot1.png)

![Screenshot](./doc/Screenshot2.png)

## Features

- Adjustable speed
- Different sart patterns
- Show count of living cells
- Show number of cycles
- Detection for end of simulation
- Dynamic adjustment to changed terminal size
- Different ui styles of living cells

## Usage

- "q" key ends the program
- "Up", "Down" and "0" through "9" keys adjust the speed
- "Right" and "Left" keys initialize the cells to a different pattern
  - "r" Random
  - "b" Blinker
  - "g" Glider
  - "l" Glider gun
  - "p" Pentomino
  - "d" Diehard
  - "a" Acorn
- "Space" key restarts the current pattern
- "h" show help
- "s" key changes the style
  - 2 chars representing 1 cell (Unicode blocks / "#")
  - 1 char represents 2 cells (Unicode braille dots / Unicode half blocks / ASCII)
  - 1 char represents 8 cells (Unicode braille dots)
- "m" key changes the mode
  - NEXT: Jump to next pattern
  - LOOP: Restart current pattern
  - STOP: Stop when pattern is finished

## Background

<https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life>

## License

MIT License, see "License.txt".

## Author

By domo
