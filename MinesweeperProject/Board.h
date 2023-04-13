#pragma once

#include "DataType.h"
#include "Handler.h"

class Board {
private:
    BoardArgs boardArgs;

    void initializeBoards();

    void calculateAnswerByInput(std::ifstream &inputFile);

    void randomTheBoard();

    void plusOneAroundTheMine(int x, int y);

    void countBlank();

    bool isCoordinateValid(int x, int y) const;

    void showMine();

    void revealGrid(int x, int y);

public:

    Board();
    
     void load(const char* path);

    void load(int inputRow, int inputColumn, double randomRate);

    void load(int inputRow, int inputColumn, int mineCount);

	void startGame();

    void maskBoard();

    void leftClick(int x, int y);

    void rightClick(int x, int y);

    const BoardArgs &getBoardArgs() const;

    void print(int printOption) const;

};