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
const int STANDARD_BOGGLE_SIZE = 4;
const int BIG_BOGGLE_SIZE = 5;
const int MIN_WORD = 4;

const string STANDARD_CUBES[16]  = {
    "AAEEGN", "ABBJOO", "ACHOPS", "AFFKPS",
    "AOOTTW", "CIMOTU", "DEILRX", "DELRVY",
    "DISTTY", "EEGHNW", "EEINSU", "EHRTVW",
    "EIOSST", "ELRTTY", "HIMNQU", "HLNNRZ"
};

const string BIG_BOGGLE_CUBES[25]  = {
    "AAAFRS", "AAEEEE", "AAFIRS", "ADENNN", "AEEEEM",
    "AEEGMU", "AEGMNN", "AFIRSY", "BJKQXZ", "CCNSTW",
    "CEIILT", "CEILPT", "CEIPST", "DDLNOR", "DDHNOT",
    "DHHLOR", "DHLNOR", "EIIITT", "EMOTTT", "ENSSSU",
    "FIPRSY", "GORRVW", "HIPRRY", "NOOTUW", "OOOTTU"
};

/* Function prototypes */

void welcome();
void giveInstructions();
bool isPermitted(string prompt);
bool isValidReply(string userInput);
void playBoggle();
Grid<char> getBoardSize(bool isBigBoggle);
void configureBoard(Grid<char> & grid, bool isManualConfig);
void humanTurn(Grid<char> & grid, Set<string> & used, Lexicon & english);
void computerTurn(Grid<char> & grid, Set<string> & usedWords, Lexicon & english);
Vector<string> manualConfig(Grid<char> & grid);
Vector<string> autoConfig(Grid<char> & grid);
void fillGrid(Grid<char> & grid, Vector<string> & letters);
Vector<string> copyCubes(Grid<char> & grid);
string validateConfigInput(string boardConfig, int nChars);
bool isAlphaString(string input);
bool isBoggleWord(Grid<char> & grid, string candidate);
bool isWordPath(Grid<char> & grid, string candidate, int row, int col, string usedCells,
                bool pastFirstLetter);
bool isAdjacentCube(Grid<char> & grid, string candidate, int row, int col, string usedCells,
                    bool flag);
void highlightUsedCubes(string cells, bool flag);
void findRemainingWords(Grid<char> & grid, Set<string> & usedWords, Lexicon & english,
                        string candidate, int row, int col, string usedCells,
                        bool pastFirstLetter);
void appendAdjacentLetters(Grid<char> & grid, Set<string> & usedWords, Lexicon & english,
                         string candidate, int row, int col, string usedCells,
                         bool pastFirstLetter);

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
    cout << "You can choose standard Boggle (4x4 grid) or Big Boggle (5x5)." << endl;
    Grid<char> grid = getBoardSize(isPermitted("Would you like Big Boggle? "));
    Set<string> usedWords;
    Lexicon english("EnglishWords.dat");
    cout << "I'll give you a chance to set up the board to your specification, which makes"
         << " it easier to confirm your boggle program is working." << endl;
    configureBoard(grid, isPermitted("Do you want to force the board configuration? "));
    humanTurn(grid, usedWords, english);
    computerTurn(grid, usedWords, english);
}

/*
 * Function: computerTurn
 * Usage: void computerTurn(grid, usedWords, english);
 * ---------------------------------------------------
 * Wrapper function that passes additional arguments to a function that finds all the
 * remaining words on the board.
 */

void computerTurn(Grid<char> & grid, Set<string> & usedWords, Lexicon & english) {
    findRemainingWords(grid, usedWords, english, "", 0, 0, "", false);
}

/*
 * Function: findRemainingWords
 * Usage: void findRemainingWords(grid, usedWords, english, candidate, row, col, usedCells);
 * -----------------------------------------------------------------------------------------
 * Finds all the remaining words on the board.
 */

void findRemainingWords(Grid<char> & grid, Set<string> & usedWords, Lexicon & english,
                        string candidate, int row, int col, string usedCells,
                        bool pastFirstLetter) {
    if (english.contains(candidate) && !usedWords.contains(candidate)
            && candidate.length() >= MIN_WORD) {
        usedWords.add(candidate);
        recordWordForPlayer(candidate, COMPUTER);
    }
    if (!pastFirstLetter) {
        for (int i = 0; i < grid.numRows(); i++) {
            for (int j = 0; j < grid.numCols(); j++) {
                findRemainingWords(grid, usedWords, english, candidate, i, j, usedCells, true);
            }
        }
    }
    if (grid.inBounds(row, col)) {
        string next = candidate + grid[row][col];
        string cell = "(" + integerToString(row) + "," + integerToString(col) + ")";
        if (english.containsPrefix(next) && usedCells.find(cell) == string::npos) {
            appendAdjacentLetters(grid, usedWords, english, next, row, col, usedCells + cell, true);
        }
    }
}

/*
 * Function adjacentLettersPath
 * Usage: void adjacentLettersPath(grid, usedWords, english, candidate, row, col, usedCells);
 * ------------------------------------------------------------------------------------------
 * Extends the word path by appending each letter adjacent to the character at the specified
 * row and column to candidate one at a time.
 */

void appendAdjacentLetters(Grid<char> & grid, Set<string> & usedWords, Lexicon & english,
                         string candidate, int row, int col, string usedCells,
                         bool pastFirstLetter) {
    findRemainingWords(grid, usedWords, english, candidate, row, col - 1, usedCells, true);
    findRemainingWords(grid, usedWords, english, candidate, row, col + 1, usedCells, true);
    findRemainingWords(grid, usedWords, english, candidate, row - 1, col, usedCells, true);
    findRemainingWords(grid, usedWords, english, candidate, row - 1, col - 1, usedCells, true);
    findRemainingWords(grid, usedWords, english, candidate, row - 1, col + 1, usedCells, true);
    findRemainingWords(grid, usedWords, english, candidate, row + 1, col, usedCells, true);
    findRemainingWords(grid, usedWords, english, candidate, row + 1, col - 1, usedCells, true);
    findRemainingWords(grid, usedWords, english, candidate, row + 1, col + 1, usedCells, true);
}

/*
 * Function: humanTurn
 * Usage: void humanTurn(used, english);
 * -------------------------------------
 * Allows the user to enter all the words they can find on the board, one line at a time
 * and to try again if an invalid guess is entered. For each word the user guesses
 * correctly, the cubes for that word are highlighted on the board and the player
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
 * the given candidate word can be formed on the board, or false otherwise.
 */

bool isBoggleWord(Grid<char> & grid, string candidate) {
    return isWordPath(grid, candidate, 0, 0, "", false);
}

/*
 * Function: isWordPath
 * Usage: bool isPath = isWordPath(grid, candidate, row, col, foundFirstLetter);
 * -----------------------------------------------------------------------------
 * Returns true if a distinct path between adjacent letters forming the specified candidate
 * word exists on the board, or false otherwise.
 */

bool isWordPath(Grid<char> & grid, string candidate, int row, int col,
              string usedCells, bool pastFirstLetter) {
    if (candidate.length() == 0) {
        highlightUsedCubes(usedCells, true);
        return true;
    }
    if (!pastFirstLetter) {
        for (int i = row; i < grid.numRows(); i++) {
            for (int j = col; j < grid.numCols(); j++) {
                if (grid[i][j] == candidate[0]) {
                    string cell = "(" + integerToString(i) + "," + integerToString(j) + ")";
                    if (isAdjacentCube(grid, candidate.substr(1), i, j, cell, true)) {
                        return true;
                    }
                }
            }
        }
    }
    if (grid.inBounds(row, col)) {
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
 * Function: highlightUsedCubes
 * Usage: void highlightUsedCubes(cells, flag);
 * --------------------------------------------
 * Highlights the cubes used to form a valid word on the board, and then
 * unhighlights them after a brief pause.
 */

void highlightUsedCubes(string cells, bool flag) {
    int cellStart = cellStart = cells.find("(");
    while (cellStart != string::npos) {
        int separatorPos = cells.find(",", cellStart);
        int rowEndPos = separatorPos - 1;
        int row = stringToInteger(cells.substr(cellStart + 1, rowEndPos - cellStart));
        int colEndPos = cells.find(")", separatorPos) - 1;
        int col = stringToInteger(cells.substr(separatorPos + 1, colEndPos - separatorPos));
        highlightCube(row, col, flag);
        cellStart = cells.find("(", cellStart + 1);
    }
    if (flag == true) {
        pause(500);
        highlightUsedCubes(cells, false);
    }
}

/*
 * Function: getBoardSize
 * Usage: Grid<char> grid = getBoardSize(isBigBoggle);
 * ---------------------------------------------------
 * Returns a grid with the dimensions of a big Boggle board (5x5) if the boolean flag is true,
 * or a standard board (4x4) if false.
 */

Grid<char> getBoardSize(bool isBigBoggle) {
    Grid<char> result;
    if (isBigBoggle) result.resize(BIG_BOGGLE_SIZE, BIG_BOGGLE_SIZE);
    else result.resize(STANDARD_BOGGLE_SIZE, STANDARD_BOGGLE_SIZE);
    return result;
}

/*
 * Function: configureBoard
 * Usage: void configureBoard(isManualConfig);
 * -------------------------------------------
 * Configures the cubes on the board. This implementation uses a boolean flag which
 * is initialized via a prompt that if true, allows the user to manually configure the
 * cubes on the board. Otherwise, the cubes on the board are automatically arranged.
 */

void configureBoard(Grid<char> & grid, bool isManualConfig) {
    Vector<string> config;
    if (isManualConfig) {
        config = manualConfig(grid);
    } else {
        config = autoConfig(grid);
    }
    drawBoard(grid.numRows(), grid.numCols());
    fillGrid(grid, config);
}

/*
 * Function: manualConfig
 * Usage: void manualConfig(grid);
 * -------------------------------
 * Takes a string of letters from the user and assigns each character to a cube on
 * the board from left to right, top to bottom. The number of cubes required is
 * determined using the dimensions of the specified grid.
 */

Vector<string> manualConfig(Grid<char> & grid) {
    Vector<string> vec;
    int nChars = grid.numRows() * grid.numCols();
    cout << "Enter a " << nChars << "-character string to identify which letters"
         << " you want on the cubes. The first " << grid.numRows()
         << " letters are the cubes on the top row from left to right, the next "
         << grid.numRows() << " letters are the second row and so on." << endl;
    string letters = validateConfigInput(getLine("Enter the string: "), nChars);
    for (int i = 0; i < letters.length(); i++) {
        vec.add(string() + letters[i]);
    }
    return vec;
}

/*
 * Function: autoConfig
 * Usage: void autoConfig(grid);
 * -----------------------------
 * Automatically configures the cubes on the board based on the dimensions of the
 * specified grid.
 */

Vector<string> autoConfig(Grid<char> & grid) {
    Vector<string> vec = copyCubes(grid);
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
 * ------------------------------------
 * Fills the upward-facing side of the cubes on the board with the specified letters
 * and stores the arrangement of the cubes in the grid reference parameter.
 */

void fillGrid(Grid<char> & grid, Vector<string> & letters) {
    int pos = 0;
    for (int i = 0; i < grid.numRows(); i++) {
        for (int j = 0; j < grid.numCols(); j++) {
            char ch = letters[pos][randomInteger(0, letters[pos].length() - 1)];
            grid[i][j] = toupper(ch);
            labelCube(i, j, toupper(ch));
            pos++;
        }
    }
}

/*
 * Function: copyCubes
 * Usage: Vector<string> vec = copyCubes(grid);
 * --------------------------------------------
 * Returns a vector containing copies of either standard or big Boggle cubes based on
 * the size of grid.
 */

Vector<string> copyCubes(Grid<char> & grid) {
    Vector<string> result;
    int nBigCubes = sizeof BIG_BOGGLE_CUBES / sizeof BIG_BOGGLE_CUBES[0];
    int nStandardCubes = sizeof STANDARD_CUBES / sizeof STANDARD_CUBES;
    if (grid.numRows() * grid.numCols() == nStandardCubes) {
        for (int i = 0; i < nStandardCubes; i++) {
            result.add(STANDARD_CUBES[i]);
        }
    } else {
        for (int i = 0; i < nBigCubes; i++) {
            result.add(BIG_BOGGLE_CUBES[i]);
        }
    }
    return result;
}

/*
 * Function: validateConfigInput
 * Usage: string letters = validateConfigInput(boardConfig, nChars);
 * -----------------------------------------------------------------
 * Checks whether the specified board configuration is in the correct format and if so,
 * returns the configuration as a string. This implementation requires the given board
 * configuration to be initialized via a prompt which takes input from the user. If the user
 * enters input in the wrong format, they are given another chance to enter the board
 * configuration.
 */

string validateConfigInput(string boardConfig, int nChars) {
    while (true) {
        if (boardConfig.length() < nChars) {
            boardConfig = getLine("String must include " + integerToString(nChars)
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
 * -------------------------------------------------
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
 * --------------------------------------------
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
