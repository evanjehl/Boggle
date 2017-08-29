#ifndef _boggle_h
#define _boggle_h

#include <iostream>
#include <string>
#include "lexicon.h"
#include "grid.h"
#include "vector.h"
#include "point.h"
using namespace std;

class Boggle {

public:
    Boggle(Lexicon& dictionary, string boardText);
    char getLetter(int row, int col);
    bool checkWord(string word);
    bool humanWordSearch(string word);
    Set<string> computerWordSearch();
    Set<string> getWordsHuman();
    int getScoreHuman();
    int getScoreComputer();
    friend ostream& operator<<(ostream& out, Boggle& boggle);

private:
    void randomBoard();
    void orderedBoard(string boardText);
    Grid<char> gameBoard;
    Lexicon boggleDictionary;
    Set<string> wordsHuman;
    Set<string> wordsComputer;
    int scoreHuman;
    int scoreComputer;
    bool canBeFormed(string word, int index, Point pt);
    void findWords(string path, Point pt, Set<string> &result);
    Grid<bool> marked;
    void resetMarkedGrid();
    void unmarkCube(int x, int y);
    void markCube(int x, int y);
    bool isMarked(int x, int y);
    bool outOfBounds(int x, int y);
    Point adjacentCube(Point pt, int i);
};

#endif // _boggle_h
