/*****************************************************************
* File: Board.h
 * Author: ´¿±R®¦¡BÃ¹¬ý²M
 * Create Date: 2023/04/06
 * Editor: ´¿±R®¦¡BÃ¹¬ý²M
 * Update Date: 2023/04/17
 * Description: a class to construct the minesweeper game.
 *********************************************************************/

#pragma once

#include "DataType.h"
#include "Handler.h"
#include <string>

class Board {
private:
    // a struct that store all the data
    BoardArgs boardArgs;

    // Intents: to initialize the boards.
    // Pre: none
    // Post: none
    void initializeBoards();
    
    // Intents: to calculate the answer by input file
    // Pre: the input file 
    // Post: it will throw exception when input data is invalid
    void calculateAnswerByInput(std::ifstream &inputFile);

    // Intents: to generate a board by the random rate
    // Pre: none
    // Post: none
    void randomTheBoard();
    
    // Intents: to plus one around the mine
    // Pre: the position x and y
    // Post: none
    void plusOneAroundTheMine(int x, int y);
    
    // Intents: to count the blank
    // Pre: none
    // Post: the result will be stored to boardArgs
    void countBlank();

    // Intents: to check if the coordinate is valid or not
    // Pre: the position x and y
    // Post: return true if it is valid or false otherwise
    bool isCoordinateValid(int x, int y) const;

    // Intents: all the mine in game board when step on the mind
    // Pre: none
    // Post: none
    void showMine();

    // Intents: to reveal nearby grid when left clicked
    // Pre: the clicked coordinate x and y
    // Post: none
    void revealGrid(int x, int y);

public:
    // Intents: default constructor
    // Pre: none
    // Post: none
    Board();

    // Intents: load the board by file
    // Pre: a string of the file path
    // Post: it will throw exception when some data is invalid
    void load(const char* path);

    // Intents: load the board by random rate
    // Pre: a string of the file path
    // Post: it will throw exception when some data is invalid
    void load(int inputRow, int inputColumn, double randomRate);

    // Intents: load the board by random count
    // Pre: a string of the file path
    // Post: it will throw exception when some data is invalid
    void load(int inputRow, int inputColumn, int mineCount);
    
    // Intents: submit the score to the leader board
    // Pre: a string of the username and a double of the playing time
    // Post: none
    void submitScore(const std::string& username, double time) const;

    // Intents: to start the game
    // Pre: none
    // Post: it will throw exception when some data is invalid
	void startGame();

    // Intents: to re-mask the board for the replay feature in gui (normal mode)
    // Pre: none
    // Post: none
    void maskBoard();

    // Intents: to left click the board
    // Pre: the click coordinate x and y
    // Post: it will throw exception when some data is invalid
    void leftClick(int x, int y);

    // Intents: to right click the board
    // Pre: the click coordinate x and y
    // Post: it will throw exception when some data is invalid
    void rightClick(int x, int y);
    
    // Intents: to get the const reference of the boardArgs 
    // Pre: none
    // Post: it returns the const reference of the boardArgs 
    const BoardArgs &getBoardArgs() const;

    // Intents: the print the selected data.
    // Pre: the selection
    // Post: print the selected data
    void print(int printOption) const;

};