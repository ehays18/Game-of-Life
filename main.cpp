// HW5-CS1337-Ethan-Hays

//Change Log
//  Originally had the loop in the main method based only on the number of generations. Added a flag for when the user presses S
//      to signal the loop to stop and print the grid to a file.
//  Created a method to use 2 arrays of integers that signify different directions to check each neighbor of each cell, instead of
//      8 large if statements in the nextGen() method

//Special Features:
//  Made it so that when a pattern file is read in, the dimensions of the pattern are taken into account so that the pattern
//      will be placed into the middle of the grid, instead of at the edge.
//  Added the ability for the user to slow down or speed up the rate at which the grid prints the next generation
//  User is given multiple chances to open a file if it does not work the first time

#include <iostream>
#include <conio.h> //for _kbhit()
#include <windows.h> //for system("cls") to clear the screen
#include <cstdlib> //for srand()
#include <ctime> //for Sleep() and time()
#include <fstream> //for file input and output
#include <vector> //for the file input setup to store the lines from the file

using namespace std;

//globals
const int MAX_GENS = 10000; //maximum number of generations the game will cycle through

const int MAX_ROWS = 50; //the maximum dimensions of the grid
const int MAX_COLS = 50;

int sleepAmt = 175; //the amount of time for the program to rest in between calls to print the board

bool gameOver = false; //flag for if the user presses the 'S' key

//directions  N, E, S, W, NE, SE, SW, NW
int rowDir[] = {-1, 0, 1, 0, -1, 1, 1, -1}; //contain the vertical and horizontal movement values for each direction in the matrix
int colDir[] = {0, 1, 0, -1, 1, 1, -1, -1};

//attempts to open the file given.
//If successful, open the file and return true
//If unsuccessful, allow the user to try a different file name, or quit the program
bool openForReading(ifstream &infile, string fileName)
{
    infile.open(fileName); //open the file with the given file name
    if(infile.good() == false) { //if unsuccessful in opening, tell the user, and prompt for a new name
        perror("can't open file");
        cout << "file name was: " << fileName << endl;
        cout << "Input another name(or quit): " << endl;
        cin >> fileName;
        if(fileName != "quit") { //if the user does not choose to quit, call openForReading on the new file given
            cout << "the new file name is: " << fileName << endl;
            openForReading(infile, fileName);
            return true;
        }
        else exit(EXIT_FAILURE); //if the user decides to quit, exit the program
    }
    else return true;
}

//determines whether the user wishes to open a pattern file, or give a percentage
//of the board to randomly initialize
int setupType(ifstream &infile, int &percentage)
{
    int option;
    cout << "Press 1 to input a pattern file. \nPress 2 to give a percentage. \nPress any other key to quit." << endl;
    cin >> option;
    if(option == 1) {
        string fileName;
        cout << "Enter the name of the file to open: " << endl;
        cin >> fileName;
        if(!openForReading(infile, fileName)) //open the file for reading, and make sure it is successful
            exit(EXIT_FAILURE); //if the user decides to quit, exit the program
    }
    else if(option == 2) {
        cout << "What percentage of the grid cells would you like to initialize: " << endl;
        cin >> percentage; //store the percentage of the board to randomly initialize
    }
    else {
        exit(EXIT_SUCCESS);
    }
    return option; //return the choice of the user to the call of the function
}

//places the pattern from the input file into the grid
void fileInputSetup(ifstream &infile, int grid1[MAX_ROWS][MAX_COLS])
{
    vector<string> filePattern; //stores the lines that hold the pattern
    string line; //stores the current line of the file
    while(getline(infile, line)) {
        if(line[0] == '#') { //if the line is a comment, print it out to the user
            cout << line << endl;
        }
        else
            filePattern.push_back(line); //otherwise, place it into the vector
    }
    int numRows = filePattern.size(); //the length of the vector will equal the number of rows
    int numCols = 0;
    for(string item : filePattern) { //find the maximum length of a line for calculating the starting column position
        int len = item.length();
        if(len > numCols)
            numCols = len;
    }

    int startingRowIndex = (((MAX_ROWS - numRows) / 2) - 1); //set the starting positions so that the pattern will
    int startingColIndex = (((MAX_COLS - numCols) / 2) - 1); //appear in the center of the grid

    int r = startingRowIndex;
    int c = startingColIndex;

    for(string currRow : filePattern) { //transfer the pattern of each line in the vector to the grid
        int length = currRow.length();
        for(int i=0; i<length; i++) {
            if(currRow[i] == '.') //if it is a '.' then the cell is dead
                grid1[r][c] = 0;
            if(currRow[i] == 'O') //if it is an 'O' then the cell is alive
                grid1[r][c] = 1;
            c++;
        }
        c = startingColIndex;
        r++;
    }

    infile.close(); //closes the input file after the reading is done
    return;
}

//randomly assign cells to dead or alive based on the percentage given by the user
void percentageSetup(int grid1[MAX_ROWS][MAX_COLS], int percentage)
{
    srand(time(NULL)); //seed the random function so that it is closer to truly random
    for(int r=0; r<MAX_ROWS; r++) {
        for(int c=0; c<MAX_COLS; c++) {
            int chanceAlive = ((rand() % 100) + 1); //generates a number between 1 and 100
            if(chanceAlive <= percentage) //if the random number falls within the percentage given,
                grid1[r][c] = 1;          //then set the cell to alive
            else
                grid1[r][c] = 0; //otherwise, set the cell as dead
        }
    }
    return;
}

//checks the cell to each direction from the current cell to see if it is alive or dead
bool directionCheck(int grid1[MAX_ROWS][MAX_COLS], int row, int col, int rowModifier, int colModifier)
{
    bool isAlive = false; //flag for if the modified position is alive or not
    //NORTHWEST
    if((rowModifier == -1) && (colModifier == -1)) { //checks each possibility of the modifiers for each direction
        if((row > 0) && (col > 0) && (grid1[row + rowModifier][col + colModifier] == 1)) //checks if the modified position is within bounds
            isAlive = true;                                                              //and whether the modified position is alive or not
    }
    //NORTHEAST
    else if((rowModifier == -1) && (colModifier == 1)) {
        if((row > 0) && (col <= MAX_COLS) && (grid1[row + rowModifier][col + colModifier] == 1))
            isAlive = true;
    }
    //NORTH
    else if((rowModifier == -1) && (colModifier == 0)) {
        if((row > 0) && (grid1[row + rowModifier][col + colModifier] == 1))
            isAlive = true;
    }
    //EAST
    else if((rowModifier == 0) && (colModifier == 1)) {
        if((col <= MAX_COLS) && (grid1[row + rowModifier][col + colModifier] == 1))
            isAlive = true;
    }
    //SOUTH
    else if((rowModifier == 1) && (colModifier == 0)) {
        if((row <= MAX_ROWS) && (grid1[row + rowModifier][col + colModifier] == 1))
            isAlive = true;
    }
    //WEST
    else if((rowModifier == 0) && (colModifier == -1)) {
        if((col > 0) && (grid1[row + rowModifier][col + colModifier] == 1))
            isAlive = true;
    }
    //SOUTHWEST
    else if((rowModifier == 1) && (colModifier == -1)) {
        if((row <= MAX_ROWS) && (col > 0) && (grid1[row + rowModifier][col + colModifier] == 1))
            isAlive = true;
    }
    //SOUTHEAST
    else if((rowModifier == 1) && (colModifier == 1)) {
        if((row <= MAX_ROWS) && (col <= MAX_COLS) && (grid1[row + rowModifier][col + colModifier] == 1))
            isAlive = true;
    }

    return isAlive;
}

//sets the orientation of the second grid to the next generation of the first grid
void nextGen(int grid1[MAX_ROWS][MAX_COLS], int grid2[MAX_ROWS][MAX_COLS])
{
    for(int r=0; r<MAX_ROWS; r++) { //go through each element of the matrix
        for(int c=0; c<MAX_COLS; c++) {
            int neighborCount = 0; //keeps count of the number of neighbors the current cell has

            for(int i=0; i<8; i++) { //iterates through each direction in the rowDir and colDir arrays and calls the directionCheck()
                if(directionCheck(grid1, r, c, rowDir[i], colDir[i])) //function to see the state of each neighboring cell
                    neighborCount++; //if the cell in the current direction is alive, increment the count of neighbors by 1
            }

            if((grid1[r][c] == 1) && (neighborCount < 2)) //if the cell is alive, but has less than 2 neighbors,
                grid2[r][c] = 0;                            //it dies in the next generation
            else if((grid1[r][c] == 1) && (neighborCount >= 4)) //if the cell is alive, but has 4 or more neighbors,
                grid2[r][c] = 0;                                    //it dies in the next generation
            else if((grid1[r][c] == 1) && (neighborCount >= 2) && (neighborCount < 4)) //if the cell is alive and has 2-3 neighbors,
                grid2[r][c] = 1;                                                        //then it remains alive in the next generation
            else if((grid1[r][c] == 0) && (neighborCount == 3)) //if the cell is dead, but has exactly 3 neighbors,
                grid2[r][c] = 1;                                //then it becomes alive in the next generation
            else
                grid2[r][c] = 0; //otherwise the cell is dead and remains dead for the next generation

        }
    }
    return;
}

//prints out the matrix passed in
void printBoard(int grid1[MAX_ROWS][MAX_COLS])
{
    system("cls"); //clears the screen before printing anything
    cout << "Press P to pause the program\nPress SPACE to resume the program\nPress S to stop the program\n"
        "Press - to slow the refresh rate of the screen\nPress + to speed up the refresh rate" << endl; //outputs the directions to the user
    for(int r=0; r<MAX_ROWS; r++) {
        for(int c=0; c<MAX_COLS; c++) { //iterates through each element of the matrix
            if(grid1[r][c] == 1)
                cout << "O"; //if the cell is alive, print out an 'O'
            else
                cout << "."; //if the cell is dead, print out a '.'
        }
        cout << endl; //put each row on a new line
    }
    return;
}

//retrieves any keyboard commands given by the user
void kbInput()
{
    if(_kbhit()) { //if the keyboard is hit
        switch(_getch()) {
            case 'p': //if the user presses P, pause the simulation until another key is pressed
                while(!_kbhit());
                break;
            case 's': //if the user presses S, stop the simulation and set the gameOver flag to true
                gameOver = true;
                break;
            case '-': //if the user presses -, slow down the refresh rate of the screen
                sleepAmt += 25;
                break;
            case '+': //if the user presses +, speed up the refresh rate of the screen
                if(sleepAmt > 25)
                    sleepAmt -= 25;
                break;
            default:
                break;
        }
    }
    return;
}

//prints the current state of the board to an output file
void printToFile(ofstream &outfile, int grid1[MAX_ROWS][MAX_COLS])
{
    outfile.open("LifeGrid-Final.txt");

    for(int r=0; r<MAX_ROWS; r++) {
        for(int c=0; c<MAX_COLS; c++) { //iterates through each element of the matrix
            if(grid1[r][c] == 1)
                outfile << "O"; //if the cell is alive, print out an 'O'
            else
                outfile << "."; //if the cell is dead, print out a '.'
        }
        outfile << endl; //print on a new line for each row
    }

    outfile.close(); //close the file after printing
    return;
}

//sets all cells of the grid to be dead for the beginning of the simulation in case they are not already
void clearGrid(int grid1[MAX_ROWS][MAX_COLS])
{
    for(int r=0; r<MAX_ROWS; r++) {
        for(int c=0; c<MAX_COLS; c++) {
            grid1[r][c] = 0; //set each element to be dead
        }
    }
    return;
}

int main()
{
    ifstream infile;
    ofstream outfile;
    int percentage = 0;
    int option = setupType(infile, percentage); //either opens an input file, or sets percentage to the value given by the user

    int grid1[MAX_ROWS][MAX_COLS]; //create the board for the simulation
    int grid2[MAX_ROWS][MAX_COLS];

    clearGrid(grid1); //clear the grids of any alive cells before they are set to a pattern, or randomly initialized
    clearGrid(grid2);

    if(option == 1)
        fileInputSetup(infile, grid1); //if a file was given, transfer the pattern given onto the grid
    else
        percentageSetup(grid1, percentage); //if a percentage was given, randomly set the given percentage of cells to be alive

    printBoard(grid1); //print out the initial state of the grid

    int genNumber = 0; //stores the current number of generations cycled through
    while((!gameOver) && (genNumber < MAX_GENS)) { //while the user has not stopped the simulation, and while the number of generations
        nextGen(grid1, grid2); //generate the next generation onto the second grid                             //is within the maximum
        printBoard(grid2); //print out the next generation
        Sleep(sleepAmt); //pause for a brief moment to allow for better visualization between generations
        nextGen(grid2, grid1); //generate the next generations onto the original grid, and continue to switch between the two
        printBoard(grid1); //print out the next generation
        Sleep(sleepAmt);
        kbInput(); //detect any keyboard commands from the user
        genNumber += 2; //increment the number of generations passed
    }

    if(gameOver) //if the user manually stopped the simulation, print the final state of the board to an output file
        printToFile(outfile, grid1);

    return 0;
}
