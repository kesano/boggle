/*
 * File: Boggle.cpp
 * ----------------
 * This program runs the game of Boggle.
 */
 
#include <iostream>
#include <cctype>
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

/* Main program */

int main() {
    GWindow gw(BOGGLE_WINDOW_WIDTH, BOGGLE_WINDOW_HEIGHT);
    initGBoggle(gw);
    welcome();
    if (isPermitted("Do you need instructions? ")) giveInstructions();
    while (true) {

        if (!isPermitted("Would you like to play again? ")) break;
    }
    return 0;
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
