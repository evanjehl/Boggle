#include "Boggle.h"
#include "grid.h"
#include "strlib.h"
#include "point.h"
#include "bogglegui.h"

// letters on all 6 sides of every cube
static string CUBES[16] = {
    "AAEEGN", "ABBJOO", "ACHOPS", "AFFKPS",
    "AOOTTW", "CIMOTU", "DEILRX", "DELRVY",
    "DISTTY", "EEGHNW", "EEINSU", "EHRTVW",
    "EIOSST", "ELRTTY", "HIMNQU", "HLNNRZ"
};

// loads dictionary, sets initial scores to 0,
// creates two 4x4 grids corresponding to letter
// and marked status of each point on game board
// initializes letters on board as either random
// or according to user input
Boggle::Boggle(Lexicon& dictionary, string boardText) {
    srand(time(NULL));
    boggleDictionary = dictionary;
    scoreHuman = 0;
    scoreComputer = 0;
    gameBoard = Grid<char>(4, 4);
    marked = Grid<bool>(4, 4);
    for (int x = 0; x < marked.numRows(); x++) {
        for (int y = 0; y < marked.numCols(); y++) {
            marked[x][y] = false;
        }
    }
    if (boardText == "") {
        randomBoard();
    } else {
        orderedBoard(toUpperCase(boardText));
    }
    resetMarkedGrid();
}

// returns letter at a specified row and column
char Boggle::getLetter(int row, int col) {
    return gameBoard[row][col];
}

// checks user's and computer's words
// against 4 parameters external to board validity:
// 1) word length must be at least 4 letters
// 2) dictionary as read from text file must contain word
// 3) user's list of scored words does not already contain this word
// 4) computer's list of scored words does not already contain this word (trivial during the user's turn)
// returns false if any of these parameters fails, true if all are met
bool Boggle::checkWord(string word) {
    return word.length() >= 4 && boggleDictionary.contains(toLowerCase(word)) && !(wordsHuman.contains(toUpperCase(word))) && !(wordsComputer.contains(toUpperCase(word)));
}

// checks board validity of user's word by cycling
// through each of the letters on the board and performing
// a recursive search from each of them, provided by the helper
// function canBeFormed.
// if canBeFormed returns true at any point, the user's word is
// added to their list, the score for the word based on length
// is added to the user's score, and the function returns true.
bool Boggle::humanWordSearch(string word) {
    resetMarkedGrid();
    word = toUpperCase(word);
    for (int x = 0; x < gameBoard.numRows(); x++) {
        for (int y = 0; y < gameBoard.numCols(); y++) {
            Point pt = Point(x, y);
            if (Boggle::canBeFormed(word, 0, pt)) {
                scoreHuman += word.length() - 3;
                wordsHuman.add(word);
                return true;
            }
        }
    }
    return false;
}

// a recursive algorithm that checks whether the user's word
// can be formed on the board.
// first checks if current point is out of bounds, then if
// letter corresponding to point is equal to letter of word
// currently being checked or is already marked.
// if letters are equal and point is not marked, then point is
// marked (both on grid and on GUI board).
// if the current point has been successfully checked against
// the last letter, function returns true.
// otherwise, function is called again until base case fails,
// at which time point is unmarked (both on grid and on GUI board).
bool Boggle::canBeFormed(string word, int index, Point pt) {
    int x = pt.getX();
    int y = pt.getY();
    if (outOfBounds(x, y)) {
        return false;
    }
    if (word[index] != gameBoard[x][y] || (isMarked(x, y) && !(index == 0))) {
        return false;
    } else {
        markCube(x, y);
        BoggleGUI::setHighlighted(x, y, true);
        if (index == int(word.length() - 1)) {
            return true;
        } else {
            index++;
            for (int i = 0; i < 8; i++) {
                if (canBeFormed(word, index, adjacentCube(pt, i))) return true;
            }
        }
        unmarkCube(x, y);
        BoggleGUI::setHighlighted(x, y, false);
    }
    return false;
}

// returns the user's current score
int Boggle::getScoreHuman() {
    return scoreHuman;
}

// returns a list of the user's scored words
Set<string> Boggle::getWordsHuman() {
    return wordsHuman;
}

// searches for all valid English words on the board
// that the user has not found, cycling through each
// letter on the board and applying a recursive algorithm
// provided by the helper function findWords.
// returns a list of words found
Set<string> Boggle::computerWordSearch() {
    Set<string> result;
    for (int x = 0; x < gameBoard.numRows(); x++) {
        for (int y = 0; y < gameBoard.numCols(); y++) {
            string path = "";
            Point pt = Point(x, y);
            findWords(path, pt, result);
        }
    }
    return result;
}

// a recursive algorithm that searches for valid English words
// on the current Boggle grid.
// first checks whether given pt is out of bounds relative to the grid,
// then if cube is marked.
// if not out of bounds and not marked, the cube is marked, the letter
// is added to the path string, and path string is run through
// checkWord function to determine word validity and use. if checkWord
// returns true, word is added to set of computer's scoring words,
// and score of word is added to computer's score.
void Boggle::findWords(string path, Point pt, Set<string> &result) {
    int x = pt.getX();
    int y = pt.getY();
    if (!outOfBounds(x, y)) {
        if (!isMarked(x, y)) {
            Boggle::markCube(x, y);
            path += gameBoard[x][y];
            if (checkWord(path)) {
                result.add(path);
                wordsComputer.add(path);
                scoreComputer += (path.length() - 3);
            }
            for (int i = 0; i < 8; i++) {
                findWords(path, adjacentCube(pt, i), result);
            }
            Boggle::unmarkCube(x, y);
        }
    }
}

// returns computer's current score
int Boggle::getScoreComputer() {
    return scoreComputer;
}

// overloads the output operator, printing the
// game board in a string four letters wide and
// four lines deep
ostream& operator<<(ostream& out, Boggle& boggle) {
    for (int i = 0; i < 4; i++) {
        string stream = "";
        for (int j = 0; j < 4; j++) {
            stream += boggle.getLetter(i, j);
        }
        out << stream << endl;
    }
    return out;
}

// generates a random board by simulating the rolling
// of the 16 dice.
// two random integers between 0 and 3, representing the
// x- and y-coordinates on the board, are designated to
// each of the "cubes" in the string set CUBES, whereupon
// a random integer between 0 and 5 determines the letter
// on the cube.
// a loop ensures that the place on the board for the cube
// is not already taken by checking if the board grid's
// value is the initial value of 0.
void Boggle::randomBoard() {
    for (int i = 0; i < 16; i++) {
        string die = CUBES[i];
        int x;
        int y;
        while (true) {
            x = randomInteger(0, 3);
            y = randomInteger(0, 3);
            if (gameBoard[x][y] == 0) break;
        }
        gameBoard[x][y] = die[randomInteger(0, 5)];
    }
}

// generates a board according to user input, placing the
// letters left to right row by row.
void Boggle::orderedBoard(string boardText) {
    int k = 0;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            gameBoard[i][j] = boardText[k++];
        }
    }
}

// returns a cube adjacent to cube corresponding
// to point in argument and an integer between 0 and 7
// linked with a specific lateral and vertical direction.
// coordinate system is defined to cycle counter-clockwise
// around origin cube with a standard for loop.
Point Boggle::adjacentCube(Point pt, int i) {
    int x = pt.getX();
    int y = pt.getY();
    switch(i) {
        case 0:
            return Point(x + 1, y + 1);
        case 1:
            return Point(x, y + 1);
        case 2:
            return Point(x - 1, y + 1);
        case 3:
            return Point(x - 1, y);
        case 4:
            return Point(x - 1, y - 1);
        case 5:
            return Point(x, y - 1);
        case 6:
            return Point(x + 1, y - 1);
        case 7:
            return Point(x + 1, y);
        default:
            return pt;
    }
}

// unmarks all values on the grid and clears
// highlighting on GUI game board
void Boggle::resetMarkedGrid() {
    for (int x = 0; x < marked.numRows(); x++) {
        for (int y = 0; y < marked.numCols(); y++) {
            marked[x][y] = false;
        }
    }
    BoggleGUI::clearHighlighting();
}


// marks cube that has been added to current recursive
// search path on board
void Boggle::markCube(int x, int y) {
    marked[x][y] = true;
}

// unmarks cube that has been subtracted from current
// recursive search path on board
void Boggle::unmarkCube(int x, int y) {
    marked[x][y] = false;
}

// returns true if a cube is currently marked, false otherwise
bool Boggle::isMarked(int x, int y) {
    return marked[x][y];
}

// returns true if coordinates are less than zero or greater
// than grid length
bool Boggle::outOfBounds(int x, int y) {
    return x < 0 || y < 0 || x > (gameBoard.numRows() - 1) || y > (gameBoard.numCols() - 1);
}
