#pragma once

#include <vector>
#include <fstream>

class Board {
private:
    std::vector<std::vector<char>> board;
    std::vector<std::vector<char>> answerBoard;
    int row = 0;
    int column = 0;


    void initializeBoards();

    void calculateAnswerByInput(std::ifstream &inputFile);

    void randomTheBoard(double randomRate);

    void plusOneAroundTheMine(int x, int y);

    bool isCoordinateValid(int x, int y) const;

public:

    //TODO: 1.leftClick and rightClick
     
    Board();

    explicit Board(std::ifstream &inputFile);

    Board(int inputRow, int inputColumn, double randomRate);

    Board(int inputRow, int inputColumn, int mineCount);

    int getRow() const;

    int getColumn() const;

    std::vector<std::vector<char>> getAnswer() const;

    std::vector<std::vector<char>> getCurrentBoard() const;

};