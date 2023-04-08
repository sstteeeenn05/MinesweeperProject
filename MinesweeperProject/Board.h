#pragma once

#include <vector>
#include <fstream>
#include "DataType.h"

class Board {
private:
    BoardArgs boardArgs;

    void initializeBoards();

    void calculateAnswerByInput(std::ifstream &inputFile);

    void randomTheBoard(double randomRate);

    void plusOneAroundTheMine(int x, int y);

    void countBlank();

    bool isCoordinateValid(int x, int y) const;

    void showMine();

    void revealGrid(int x, int y);

public:

    Board();
    
    explicit Board(std::ifstream &inputFile);

    Board(int inputRow, int inputColumn, double randomRate);

    Board(int inputRow, int inputColumn, int mineCount);

    void maskBoard();

    void leftClick(int x, int y);

    void rightClick(int x, int y);

    const BoardArgs &getBoardArgs() const;

};