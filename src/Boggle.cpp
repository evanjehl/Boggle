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

// letters on every cube in 5x5 "Big Boggle" version (extension)
static string BIG_BOGGLE_CUBES[25] = {
   "AAAFRS", "AAEEEE", "AAFIRS", "ADENNN", "AEEEEM",
   "AEEGMU", "AEGMNN", "AFIRSY", "BJKQXZ", "CCNSTW",
   "CEIILT", "CEILPT", "CEIPST", "DDLNOR", "DDHNOT",
   "DHHLOR", "DHLNOR", "EIIITT", "EMOTTT", "ENSSSU",
   "FIPRSY", "GORRVW", "HIPRRY", "NOOTUW", "OOOTTU"
};

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

char Boggle::getLetter(int row, int col) {
    return gameBoard[row][col];
}

bool Boggle::checkWord(string word) {
    return word.length() >= 4 && boggleDictionary.contains(toLowerCase(word)) && !(wordsHuman.contains(toUpperCase(word))) && !(wordsComputer.contains(toUpperCase(word)));
}

bool Boggle::humanWordSearch(string word) {
    resetMarkedGrid();
    word = toUpperCase(word);
    if (wordsHuman.contains(word)) return false;
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

int Boggle::getScoreHuman() {
    return scoreHuman;
}

Set<string> Boggle::getWordsHuman() {
    return wordsHuman;
}

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

void Boggle::findWords(string path, Point pt, Set<string> &result) {
    int x = pt.getX();
    int y = pt.getY();
    if (!outOfBounds(x, y)) {
        if (!(isMarked(x, y) && path.length() != 1)) {
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

int Boggle::getScoreComputer() {
    return scoreComputer;
}

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

void Boggle::orderedBoard(string boardText) {
    int k = 0;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            gameBoard[i][j] = boardText[k++];
        }
    }
}

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

void Boggle::resetMarkedGrid() {
    for (int x = 0; x < marked.numRows(); x++) {
        for (int y = 0; y < marked.numCols(); y++) {
            marked[x][y] = false;
        }
    }
    BoggleGUI::clearHighlighting();
}

void Boggle::markCube(int x, int y) {
    marked[x][y] = true;
}

void Boggle::unmarkCube(int x, int y) {
    marked[x][y] = false;
}

bool Boggle::isMarked(int x, int y) {
    return marked[x][y];
}

bool Boggle::outOfBounds(int x, int y) {
    return x < 0 || y < 0 || x > (gameBoard.numRows() - 1) || y > (gameBoard.numCols() - 1);
}
