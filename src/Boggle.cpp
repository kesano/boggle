/*
 * File: Boggle.cpp
 * ----------------
 * This program runs the game of Boggle.
 */
 
#include <iostream>
#include <cctype>
#include <string>
#include "gboggle.h"
#include "grid.h"
#include "gwindow.h"
#include "lexicon.h"
#include "random.h"
#include "simpio.h"
#include "strlib.h"
#include "set.h"
#include "vector.h"
#include "map.h"
using namespace std;

/* Constants */

const int BOGGLE_WINDOW_WIDTH = 650;
const int BOGGLE_WINDOW_HEIGHT = 350;
const int STANDARD_BOARD_SIZE = 4;
const int MIN_WORD = 4;

const string STANDARD_CUBES[16]  = {
    "AAEEGN", "ABBJOO", "ACHOPS", "AFFKPS",
    "AOOTTW", "CIMOTU", "DEILRX", "DELRVY",
    "DISTTY", "EEGHNW", "EEINSU", "EHRTVW",
    "EIOSST", "ELRTTY", "HIMNQU", "HLNNRZ"
};

/* Function prototypes */

void welcome();
void giveInstructions();
bool isPermitted(string prompt);
bool isValidReply(string userInput);
void playBoggle();
void humanTurn(Grid<char> & grid, Set<string> & used, Lexicon & english);
void configureBoard(Grid<char> & grid, bool isManualConfig);
Vector<string> manualConfig();
Vector<string> autoConfig();
void fillGrid(Grid<char> & grid, Vector<string> & letters);
Vector<string> copyCubes();
string validateConfigInput(string boardConfig, int minChars);
bool isAlphaString(string input);
bool isBoggleWord(Grid<char> & grid, string candidate);
bool isWordPath(Grid<char> & grid, string candidate, int row, int col, string usedCells,
                bool pastFirstLetter);
bool isAdjacentCube(Grid<char> & grid, string candidate, int row, int col, string usedCells,
                    bool flag);

/* Main program */

int main() {
    GWindow gw(BOGGLE_WINDOW_WIDTH, BOGGLE_WINDOW_HEIGHT);
    initGBoggle(gw);
    welcome();
    if (isPermitted("Do you need instructions? ")) giveInstructions();
    while (true) {
        playBoggle();
        if (!isPermitted("Would you like to play again? ")) break;
    }
    return 0;
}

/*
 * Function: playBoggle
 * Usage: void playBoggle();
 * -------------------------
 * Plays the game.
 */

void playBoggle() {
    Grid<char> grid(STANDARD_BOARD_SIZE, STANDARD_BOARD_SIZE);
    Set<string> usedWords;
    Lexicon english("EnglishWords.dat");
    drawBoard(STANDARD_BOARD_SIZE, STANDARD_BOARD_SIZE);
    cout << "I'll give you a chance to set up the board to your specification, which makes"
         << " it easier to confirm your boggle program is working." << endl;
    configureBoard(grid, isPermitted("Do you want to force the board configuration? "));
    humanTurn(grid, usedWords, english);
}

/*
 * Function: humanTurn
 * Usage: void humanTurn(used, english);
 * -------------------------------------
 * Allows the user to enter all the words they can find on the Boggle board, one line at a
 * time, and to try again if an invalid guess is entered. For each word the user guesses
 * correctly, the cubes for that word are highlighted on the Boggle board and the player
 * scoreboard is updated accordingly.
 */

void humanTurn(Grid<char> & grid, Set<string> & used, Lexicon & english) {
    cout << "Ok, take all the time you want and find all the words you can! Signal that"
         << " you're finished by entering an empty line." << endl;
    while (true) {
        string candidate = toUpperCase(getLine("Enter a word: "));
        if (candidate == "") break;
        if (used.contains(candidate)) {
            cout << "You've already guessed that!" << endl;
        } else if (candidate.length() < MIN_WORD) {
            cout << "I'm sorry, but we have our standards.\nThat word doesn't meet the"
                 << " minimum word length." << endl;
        } else if (english.contains(candidate)) {
            if (isBoggleWord(grid, candidate)) {
                used.add(candidate);
                recordWordForPlayer(candidate, HUMAN);
            } else {
                cout << "You can't make that word!" << endl;
            }
        } else {
            cout << "That's not a word!" << endl;
        }
    }
}

/*
 * Function: isBoggleWord
 * Usage: bool isWord = isBoggleWord(grid, candidate);
 * ---------------------------------------------------
 * Wrapper function that passes additional arguments to a function that returns true if
 * the given candidate word can be formed on the Boggle board, or false otherwise.
 */

bool isBoggleWord(Grid<char> & grid, string candidate) {
    return isWordPath(grid, candidate, 0, 0, "", false);
}

/*
 * Function: isWordPath
 * Usage: bool isPath = isWordPath(grid, candidate, row, col, foundFirstLetter);
 * -----------------------------------------------------------------------------
 * Returns true if a distinct path between adjacent letters forming the specified candidate
 * word exists on the Boggle board, or false otherwise.
 */

bool isWordPath(Grid<char> & grid, string candidate, int row, int col,
              string usedCells, bool pastFirstLetter) {
    if (candidate.length() == 0) {
        return true;
    }
    if (!pastFirstLetter) {
        for (int i = row; i < STANDARD_BOARD_SIZE; i++) {
            for (int j = col; j < STANDARD_BOARD_SIZE; j++) {
                if (grid[i][j] == candidate[0]) {
                    string cell = "(" + integerToString(i) + "," + integerToString(j) + ")";
                    if (isAdjacentCube(grid, candidate.substr(1), i, j, cell, true)) {
                        return true;
                    }
                }
            }
        }
    }
    if (row >= 0 && row < STANDARD_BOARD_SIZE && col >= 0 && col < STANDARD_BOARD_SIZE) {
        string cell = "(" + integerToString(row) + "," + integerToString(col) + ")";
        if (grid[row][col] == candidate[0] && usedCells.find(cell) == string::npos) {
            return isAdjacentCube(grid, candidate.substr(1), row, col, usedCells + cell, true);
        }
    }
    return false;
}

/*
 * Function: isAdjacentCube
 * Usage: bool isNextLetter = isAdjacentCube(grid, candidate, row, col, usedCells, flag);
 * --------------------------------------------------------------------------------------
 * Returns true if the letter on any of the cubes adjacent to the cube at the specified row and
 * column is the same as the first letter of the given candidate string.
 */

bool isAdjacentCube(Grid<char> & grid, string candidate, int row, int col,
                    string usedCells, bool flag) {
    return  isWordPath(grid, candidate, row, col - 1, usedCells, true)
            || isWordPath(grid, candidate, row, col + 1, usedCells, true)
            || isWordPath(grid, candidate, row - 1, col, usedCells, true)
            || isWordPath(grid, candidate, row - 1, col - 1, usedCells, true)
            || isWordPath(grid, candidate, row - 1, col + 1, usedCells, true)
            || isWordPath(grid, candidate, row + 1, col, usedCells, true)
            || isWordPath(grid, candidate, row + 1, col - 1, usedCells, true)
            || isWordPath(grid, candidate, row + 1, col + 1, usedCells, true);
}

/*
 * Function: configureBoard
 * Usage: void configureBoard(isManualConfig);
 * -------------------------------------------
 * Configures the cubes on the Boggle board. This implementation requires a boolean
 * parameter that is initialized via a prompt which takes input from the user.
 */

void configureBoard(Grid<char> & grid, bool isManualConfig) {
    Vector<string> config;
    if (isManualConfig) {
        config = manualConfig();
    } else {
        config = autoConfig();
    }
    fillGrid(grid, config);
}

/*
 * Function: manualConfig
 * Usage: void manualConfig();
 * ---------------------------
 * Takes a string of letters from the user and assigns each character to a cube on
 * the Boggle board from left to right, top to bottom.
 */

Vector<string> manualConfig() {
    Vector<string> vec;
    int minChars = sizeof STANDARD_CUBES / sizeof STANDARD_CUBES[0];
    cout << "Enter a " << minChars << "-character string to identify which letters you"
         << " want on the cubes. The first " << STANDARD_BOARD_SIZE << " letters are the"
         << " cubes on the top row from left to right, the next 4 letters are the second row"
         << " and so on." << endl;
    string letters = validateConfigInput(getLine("Enter the string: "), minChars);
    for (int i = 0; i < letters.length(); i++) {
        vec.add(string() + letters[i]);
    }
    return vec;
}

/*
 * Function: autoConfig
 * Usage: void autoConfig();
 * -------------------------
 * Automatically configures the Boggle cubes.
 */

Vector<string> autoConfig() {
    Vector<string> vec = copyCubes();
    for (int i = 0; i < vec.size(); i++) {
        int x = randomInteger(i, vec.size() - 1);
        string tmp = vec[x];
        vec[x] = vec[i];
        vec[i] = tmp;
    }
    return vec;
}

/*
 * Function: fillGrid
 * Usage: void fillGrid(grid, letters);
 * -----------------------------
 * Fills the upward-facing side of the cubes on the Boggle board with the specified
 * letters and stores the arrangement of the cubes in the grid reference parameter.
 */

void fillGrid(Grid<char> & grid, Vector<string> & letters) {
    int pos = 0;
    for (int i = 0; i < STANDARD_BOARD_SIZE; i++) {
        for (int j = 0; j < STANDARD_BOARD_SIZE; j++) {
            char ch = letters[pos][randomInteger(0, letters[pos].length() - 1)];
            grid[i][j] = toupper(ch);
            labelCube(i, j, toupper(ch));
            pos++;
        }
    }
}

/*
 * Function: copyCubes
 * Usage: Vector<string> vec = copyCubes();
 * ----------------------------------------
 * Returns a vector containing copies of elements from an array.
 */

Vector<string> copyCubes() {
    Vector<string> result;
    int arraySize = sizeof STANDARD_CUBES / sizeof STANDARD_CUBES[0];
    for (int i = 0; i < arraySize; i++) {
        result.add(STANDARD_CUBES[i]);
    }
    return result;
}

/*
 * Function: validateConfigInput
 * Usage: string letters = validateConfigInput(boardConfig, minChars);
 * ---------------------------------------------------------
 * Checks whether the specified board configuration is in the correct format and if so,
 * returns the configuration as a string. This implementation requires the given board
 * configuration to be initialized via a prompt which takes input from the user. If the user
 * enters input in the wrong format, they are given another chance to enter the board
 * configuration.
 */

string validateConfigInput(string boardConfig, int minChars) {
    while (true) {
        if (boardConfig.length() < minChars) {
            boardConfig = getLine("String must include " + integerToString(minChars)
                                  + " characters! Try again: ");
        } else if(!isAlphaString(boardConfig)) {
            boardConfig = getLine("String must only include letters of the english alphabet!"
                                  " Try again: ");
        } else {
            break;
        }
    }
    return boardConfig;
}

/*
 * Function: isAlphaString
 * Usage: bool isValidInput = isAlphaString(input);
 * ------------------------------------------------
 * Returns true if the specified string input contains only letters of the engligsh
 * alphabet, or false otherwise.
 */

bool isAlphaString(string input) {
    for (int i = 0; i < input.length(); i++) {
        if (!isalpha(input[i])) return false;
    }
    return true;
}

/*
 * Function: isPermitted
 * Usage: bool userConsent = isPermitted(userInput);
 * ---------------------------------
 * Prompts the user with a specified question. Returns true if the user's response
 * is any accepted variation of "yes" or false otherwise.
 */

bool isPermitted(string prompt) {
    while (true) {
        string userInput = toUpperCase(getLine(prompt));
        if (isValidReply(userInput)) {
            if (userInput[0] == 'N') {
                return false;
            }
            break;
        } else {
            cout << "Please answer \"YES\" or \"NO\"" << endl;
        }
    }
    return true;
}

/*
 * Function: isValidReply
 * Usage: bool reply = isValidReply(userInput);
 * --------------------------------------------------
 * Returns true if the specified user input meets the necessary criteria to be valid,
 * or false otherwise. This implementation requires the user input parameter to be
 * in the uppercase format.
 */

bool isValidReply(string userInput) {
    Set<string> validInputs;
    validInputs.add("Y");
    validInputs.add("N");
    validInputs.add("YES");
    validInputs.add("NO");
    if (validInputs.contains(userInput)) return true;
    return false;
}

/*
 * Function: welcome
 * Usage: welcome();
 * -----------------
 * Print out a cheery welcome message.
 */

void welcome() {
    cout << "Welcome!  You're about to play an intense game ";
    cout << "of mind-numbing Boggle.  The good news is that ";
    cout << "you might improve your vocabulary a bit.  The ";
    cout << "bad news is that you're probably going to lose ";
    cout << "miserably to this little dictionary-toting hunk ";
    cout << "of silicon.  If only YOU had a gig of RAM..." << endl << endl;
}

/*
 * Function: giveInstructions
 * Usage: giveInstructions();
 * --------------------------
 * Print out the instructions for the user.
 */

void giveInstructions() {
    cout << endl;
    cout << "The boggle board is a grid onto which I ";
    cout << "I will randomly distribute cubes. These ";
    cout << "6-sided cubes have letters rather than ";
    cout << "numbers on the faces, creating a grid of ";
    cout << "letters on which you try to form words. ";
    cout << "You go first, entering all the words you can ";
    cout << "find that are formed by tracing adjoining ";
    cout << "letters. Two letters adjoin if they are next ";
    cout << "to each other horizontally, vertically, or ";
    cout << "diagonally. A letter can only be used once ";
    cout << "in each word. Words must be at least four ";
    cout << "letters long and can be counted only once. ";
    cout << "You score points based on word length: a ";
    cout << "4-letter word is worth 1 point, 5-letters ";
    cout << "earn 2 points, and so on. After your puny ";
    cout << "brain is exhausted, I, the supercomputer, ";
    cout << "will find all the remaining words and double ";
    cout << "or triple your paltry score." << endl << endl;
    cout << "Hit return when you're ready...";
    getLine();
}
