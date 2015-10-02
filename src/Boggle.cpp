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
using namespace std;

/* Constants */

const int BOGGLE_WINDOW_WIDTH = 650;
const int BOGGLE_WINDOW_HEIGHT = 350;
const int STANDARD_BOARD_SIZE = 4;

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
void configureBoard(bool isManualConfig);
void manualConfig();
void autoConfig();
void fillGrid(Vector<string> & letters);
Vector<string> copyCubes();
string validateConfigInput(string boardConfig, int minChars);
bool isAlphaString(string input);

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
    Set<string> usedWords;
    Lexicon english("EnglishWords.dat");
    drawBoard(STANDARD_BOARD_SIZE, STANDARD_BOARD_SIZE);
    cout << "I'll give you a chance to set up the board to your specification, which makes"
         << " it easier to confirm your boggle program is working." << endl;
    configureBoard(isPermitted("Do you want to force the board configuration? "));
}

/*
 * Function: configureBoard
 * Usage: void configureBoard(isManualConfig);
 * -------------------------------------------
 * Configures the cubes on the Boggle board. This implementation requires a boolean
 * parameter that is initialized via a prompt which takes input from the user.
 */

void configureBoard(bool isManualConfig) {
    if (isManualConfig) {
        manualConfig();
    } else {
        autoConfig();
    }
}

/*
 * Function: manualConfig
 * Usage: void manualConfig();
 * ---------------------------
 * Takes a string of letters from the user and assigns each character to a cube on
 * the Boggle board from left to right, top to bottom.
 */

void manualConfig() {
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
    fillGrid(vec);
}

/*
 * Function: autoConfig
 * Usage: void autoConfig();
 * -------------------------
 * Automatically configures the Boggle cubes.
 */

void autoConfig() {
    Vector<string> vec = copyCubes();
    for (int i = 0; i < vec.size(); i++) {
        int x = randomInteger(i, vec.size() - 1);
        string tmp = vec[x];
        vec[x] = vec[i];
        vec[i] = tmp;
    }
    fillGrid(vec);
}

/*
 * Function: fillGrid
 * Usage: void fillGrid(letters);
 * -----------------------------
 * Fills the upward-facing side of the cubes on the Boggle board with the specified
 * letters.
 */

void fillGrid(Vector<string> & letters) {
    int pos = 0;
    for (int i = 0; i < STANDARD_BOARD_SIZE; i++) {
        for (int j = 0; j < STANDARD_BOARD_SIZE; j++) {
            labelCube(i, j, letters[pos][randomInteger(0, letters[pos].length() - 1)]);
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
