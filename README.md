# Game-of-Life
Conway's Game of Life

This is an implementation of John Conway's Game of Life that I programmed in C++ for my Computer Science 1 class. We were given the rules of the game, and instructed to make a program that simulated the game. In addition, the user is given the option to provide a 'seed file' with a starting position of 'living' cells as a beginning for the simulation.

The rules of the game:
  1. A 'living' cell with two or three 'neighbors' (adjacent cells, including diagonally adjacent) survives to the next generation.
  2. A 'living' cell with more than three 'neighbors' dies due to overpopulation.
  3. A 'living' cell with fewer than two 'neighbors' dies due to underpopulation.
  4. A 'non-living' cell with exactly three 'neighbors' comes to life in the next generation.
  
Unless a seed file is given, the program asks the user to give a percentage of the cells in the grid to begin as living. After given a percentage, the program picks positions of the allotted percentage of cells randomly across the grid. 
