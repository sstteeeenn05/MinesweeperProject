#include "Board.h"
#include <algorithm>
#include <random>
#include <iostream>
#include <exception>

void Board::initializeBoards() {
    board = std::vector<std::vector<char>>
            (row, std::vector<char>(column, '#'));

    answerBoard = std::vector<std::vector<char>>
            (row, std::vector<char>(column, '0'));
}

void Board::calculateAnswerByInput(std::ifstream &inputFile) {
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < column; j++) {
            if (inputFile.get() == 'X') {
                answerBoard[i][j] = 'X';
                plus1AroundTheMine(j, i);
            }
        }
        inputFile.get();
    }
}

void Board::randomTheBoard(double randomRate) {
    std::random_device dev;
    std::mt19937 generator(dev());
    const unsigned int mineRange = static_cast<int>(randomRate * 100);
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < column; j++) {
            if (generator() % 100 + 1 < mineRange) {
                answerBoard[i][j] = 'X';
                plus1AroundTheMine(j, i);
            }
        }
    }
}

void Board::plus1AroundTheMine(int x, int y) {
    for (int i = y - 1; i <= y + 1; i++) {
        for (int j = x - 1; j <= x + 1; j++) {
            if (isCoordinateValid(j, i) && answerBoard[i][j] != 'X') {
                answerBoard[i][j]++;
            }
        }
    }
}

bool Board::isCoordinateValid(int x, int y) const {
    return (x >= 0 && x < column && y >= 0 && y < row);
}

Board::Board() = default;

Board::Board(std::ifstream &inputFile) {
    inputFile >> row >> column;
    inputFile.get();
    if (row <= 0 && column <= 0) throw std::exception();
    initializeBoards();
    calculateAnswerByInput(inputFile);
    inputFile.close();
}

Board::Board(int inputRow, int inputColumn, double randomRate) : row(inputRow), column(inputColumn) {
    if (row <= 0 && column <= 0) throw std::exception();
    if(randomRate<0) throw std::exception();
    initializeBoards();
    randomTheBoard(randomRate);

}

Board::Board(int inputRow, int inputColumn, int mineCount) : row(inputRow), column(inputColumn) {
    if (row <= 0 && column <= 0) throw std::exception();
    if (mineCount > row * column) throw std::exception();
    initializeBoards();

    std::random_device dev;
    std::mt19937 generator(dev());

    std::vector<char> tempBoard(row*column,'0');
    std::fill_n(tempBoard.begin(),mineCount,'X');
    std::shuffle(tempBoard.begin(),tempBoard.end(),generator);

    for (int i = 0; i < row; i++) {
        for (int j = 0; j < column; j++) {
            if (tempBoard[i*row+j]=='X') {
                answerBoard[i][j] = 'X';
                plus1AroundTheMine(j, i);
            }
        }
    }
}

int Board::getRow() const {
    return row;
}

int Board::getColumn() const {
    return column;
}