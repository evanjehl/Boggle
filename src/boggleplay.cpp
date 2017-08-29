#include "lexicon.h"
#include "simpio.h"
#include "bogglegui.h"
#include "console.h"
#include "boggle.h"
#include <iostream>

void printCurrentHumanState(Boggle &boggle);

void turnHuman(Boggle &boggle);

void turnComputer(Boggle &boggle);

void playOneGame(Lexicon& dictionary) {
    BoggleGUI::initialize(4, 4);
    BoggleGUI::setAnimationDelay(100);
    string boardText;
    bool randomBoard = false;
    if(getYesOrNo("Do you want to generate a random board?")) {
        boardText = "";
        randomBoard = true;
    } else {
        while (true) {
            boardText = getLine("Type the 16 letters to appear on the board: ");
            if (boardText.length() == 16) break;
            cout << "That is not a valid 16-letter board string. Try again." << endl;
        }
    }
    Boggle boggle = Boggle(dictionary, boardText);
    if (randomBoard) {
        for (int x = 0; x < 4; x++) {
            for (int y = 0; y < 4; y++) {
                BoggleGUI::labelCube(x, y, boggle.getLetter(x, y));
            }
        }
    } else {
        BoggleGUI::labelAllCubes(boardText);
    }
    clearConsole();
    turnHuman(boggle);
    turnComputer(boggle);
    string status;
    if (boggle.getScoreComputer() > boggle.getScoreHuman()) {
        status = "Ha ha ha, I destroyed you. Better luck next time, puny human!";
    } else {
        status = "WOW, you defeated me! Congratulations!";
    }
    cout << status << endl;
    BoggleGUI::setStatusMessage(status);
}

void turnHuman(Boggle &boggle) {
    cout << "It's your turn!" << endl;
    while (true) {
        cout << boggle << endl;
        cout << endl;
        printCurrentHumanState(boggle);
        string word = getLine("Type a word (or Enter to stop): ");
        if (word == "") break;
        string status;
        if (boggle.checkWord(word)) {
            if (boggle.humanWordSearch(word)) {
                BoggleGUI::recordWord(word, BoggleGUI::HUMAN);
                BoggleGUI::setScore(boggle.getScoreHuman(), BoggleGUI::HUMAN);
                status = "You found a new word! \"" + word + "\"";
            } else {
                status = "That word can't be formed on this board.";
            }
        } else {
            status = "You must enter an unfound 4+ letter word from the dictionary.";
        }
        clearConsole();
        cout << status << endl;
        BoggleGUI::setStatusMessage(status);
    }
    cout << endl;
}

void turnComputer(Boggle &boggle) {
    cout << "It's my turn!" << endl;
    Set<string> words = boggle.computerWordSearch();
    string wordList = "";
    int numWords = words.size();
    int i = 0;
    for (string word: words) {
        wordList += "\"" + word + "\"";
        if (i < numWords - 1) wordList += ", ";
        BoggleGUI::recordWord(word, BoggleGUI::COMPUTER);
        i++;
    }
    int score = boggle.getScoreComputer();
    BoggleGUI::setScore(score, BoggleGUI::COMPUTER);
    cout << "My words (" << numWords << "): {" << wordList << "}" << endl;
    cout << "My score: " << score << endl;
}

void printCurrentHumanState(Boggle &boggle) {
    Set<string> words = boggle.getWordsHuman();
    string wordList = "";
    int numWords = words.size();
    int i = 0;
    for (string word: words) {
        wordList += "\"" + word + "\"";
        if (i < numWords - 1) wordList += ", ";
        i++;
    }
    cout << "Your words (" << numWords << "): {" << wordList << "}" << endl;
    cout << "Your score: " << boggle.getScoreHuman() << endl;
}
