#include "Board.h"
#include <algorithm>
#include <random>
#include <exception>
#include <queue>
#include <fstream>
#include <ctime>

void Board::initializeBoards() {
    boardArgs.board = std::vector<std::vector<char>>
        (boardArgs.row, std::vector<char>(boardArgs.column, MINE_MASK));

    boardArgs.answer = std::vector<std::vector<char>>
        (boardArgs.row, std::vector<char>(boardArgs.column, MINE_NULL));
}

void Board::calculateAnswerByInput(std::ifstream& inputFile) {
    for (int i = 0; i < boardArgs.row; i++) {
        for (int j = 0; j < boardArgs.column; j++) {
            if (inputFile.get() == MINE_MINE) {
                boardArgs.answer[i][j] = MINE_MINE;
                boardArgs.bombCount++;
                plusOneAroundTheMine(j, i);
            }
        }
        // check column count is valid
        if (inputFile.get() != '\n' && !inputFile.eof()) throw std::exception("File format error.");

        // check row count is valid
        if (inputFile.eof() && i != boardArgs.row - 1) throw std::exception("File format error.");
    }
}

void Board::randomTheBoard() {
    std::random_device dev;
    std::mt19937 generator(dev());
    const unsigned int mineRange = static_cast<int>(boardArgs.randomRate * 100);
    for (int i = 0; i < boardArgs.row; i++) {
        for (int j = 0; j < boardArgs.column; j++) {
            if (generator() % 100 + 1 <= mineRange) {
                boardArgs.answer[i][j] = MINE_MINE;
                boardArgs.bombCount++;
                plusOneAroundTheMine(j, i);
            }
        }
    }
}

void Board::plusOneAroundTheMine(int x, int y) {
    for (int i = y - 1; i <= y + 1; i++) {
        for (int j = x - 1; j <= x + 1; j++) {
            if (isCoordinateValid(j, i) && boardArgs.answer[i][j] != MINE_MINE) {
                boardArgs.answer[i][j]++;
            }
        }
    }
}

void Board::countBlank() {
    int tempBlankCount = 0;
    for (int i = 0; i < boardArgs.row; i++) {
        for (int j = 0; j < boardArgs.column; j++) {
            if ((boardArgs.board[i][j] == MINE_MASK ||
                 boardArgs.board[i][j] == MINE_FLAG ||
                 boardArgs.board[i][j] == MINE_SUS) &&
                boardArgs.answer[i][j] != MINE_MINE)
                tempBlankCount++;
        }
    }

    boardArgs.remainBlankCount = tempBlankCount;
    boardArgs.openBlankCount = boardArgs.row * boardArgs.column - boardArgs.bombCount - boardArgs.remainBlankCount;
}

bool Board::isCoordinateValid(int x, int y) const {
    return (x >= 0 && x < boardArgs.column && y >= 0 && y < boardArgs.row);
}

void Board::showMine() {
    for (int i = 0; i < boardArgs.row; i++) {
        for (int j = 0; j < boardArgs.column; j++) {
            if (boardArgs.answer[i][j] == MINE_MINE)
                boardArgs.board[i][j] = MINE_MINE;
        }
    }
}

void Board::revealGrid(int x, int y) {
    struct Pos {
        int x;
        int y;
    };

    std::queue<Pos> queue;
    queue.push(Pos{x, y});

    while (!queue.empty()) {
        const Pos tempPos = queue.front();
        queue.pop();

        if (boardArgs.answer[tempPos.y][tempPos.x] == MINE_NULL &&
            (boardArgs.board[tempPos.y][tempPos.x] == MINE_MASK
             || boardArgs.board[tempPos.y][tempPos.x] == MINE_SUS)) {

            for (int i = tempPos.y - 1; i <= tempPos.y + 1; i++) {
                for (int j = tempPos.x - 1; j <= tempPos.x + 1; j++) {
                    if (isCoordinateValid(j, i)) {
                        if ((boardArgs.board[i][j] == MINE_MASK || boardArgs.board[i][j] == MINE_SUS))
                            queue.push(Pos{j, i});
                    }
                }
            }
        }

        if (boardArgs.answer[tempPos.y][tempPos.x] != MINE_MINE && boardArgs.board[tempPos.y][tempPos.x] != MINE_FLAG)
            boardArgs.board[tempPos.y][tempPos.x] = boardArgs.answer[tempPos.y][tempPos.x];
    }
}

Board::Board() = default;

void Board::load(const char* path) {
	if (boardArgs.state != STATE_STANDBY) throw std::exception("not Standby");
    std::ifstream inputFile(path);
    if (inputFile.fail()) throw std::exception("File does not exist");
    inputFile >> boardArgs.row >> boardArgs.column;
    inputFile.get();
    if (boardArgs.row <= 0 || boardArgs.column <= 0) {
        inputFile.close();
        throw std::exception("Row or column out of range.");
    }

    boardArgs.mode = MODE_READ_BOARD;
    boardArgs.path = path;

    initializeBoards();
    calculateAnswerByInput(inputFile);
    inputFile.close();
    countBlank();
}

void Board::load(int inputRow, int inputColumn, double randomRate) {
	if (boardArgs.state != STATE_STANDBY) throw std::exception("not Standby");
    if (inputRow <= 0 || inputColumn <= 0) throw std::exception("Row or column out of range.");
    if (randomRate < 0) throw std::exception("Random rate out of range.");
    boardArgs.mode = MODE_INPUT_RATE;
    boardArgs.randomRate = randomRate;
    boardArgs.row = inputRow;
    boardArgs.column = inputColumn;

    initializeBoards();
    randomTheBoard();
    countBlank();
}

void Board::load(int inputRow, int inputColumn, int mineCount) {
	if (boardArgs.state != STATE_STANDBY) throw std::exception("not Standby");
    if (inputRow <= 0 || inputColumn <= 0) throw std::exception("Row or column out of range.");
    if (mineCount > inputRow * inputColumn) throw std::exception("Mine count out of range.");
    boardArgs.mode = MODE_INPUT_COUNT;
    boardArgs.row = inputRow;
    boardArgs.column = inputColumn;
    boardArgs.bombCount = mineCount;

    initializeBoards();
    std::random_device dev;
    std::mt19937 generator(dev());

    std::vector<char> tempBoard(boardArgs.row * boardArgs.column, MINE_NULL);
    std::fill_n(tempBoard.begin(), mineCount, MINE_MINE);
    std::shuffle(tempBoard.begin(), tempBoard.end(), generator);

    for (int i = 0; i < boardArgs.row; i++) {
        for (int j = 0; j < boardArgs.column; j++) {
            if (tempBoard[i * boardArgs.column + j] == MINE_MINE) {
                boardArgs.answer[i][j] = MINE_MINE;
                plusOneAroundTheMine(j, i);
            }
        }
    }

    countBlank();
}

void Board::submitScore(const std::string& username, const double time) const {
    system("if not exist submit mkdir submit");
    std::ofstream leaderBoard("submit/leaderBoard.txt",std::ofstream::app);
    
    const std::string timeStr = std::to_string(std::time(nullptr)); 
    const std::string boardFilePath = "submit/" + timeStr + ".txt";
    
    std::ofstream boardFile(boardFilePath, std::ofstream::openmode());
    
    leaderBoard << timeStr << " " << username << " "<< time << " " 
                << boardArgs.row << " " << boardArgs.column << " " << boardArgs.bombCount << std::endl;
    
    boardFile<< boardArgs.row << " " << boardArgs.column << std::endl;
    
    for (const std::vector<char>& items : boardArgs.answer) {
        for (const char& item : items) {
            if(item == MINE_MINE) boardFile << 'X';
            else boardFile<<'O';
        }
        boardFile << std::endl;
    }

    leaderBoard.close();
    boardFile.close();
}


void Board::startGame() {
	if (boardArgs.row <= 0 || boardArgs.column <= 0) throw std::exception("Board is not load yet.");
	if (boardArgs.state != STATE_STANDBY) throw std::exception("wrong state");
	boardArgs.state = STATE_PLAYING;
}

void Board::maskBoard() {
    boardArgs.status = BOARD_STATUS_CONTINUE;
    boardArgs.state = STATE_STANDBY;
    std::for_each(boardArgs.board.begin(), boardArgs.board.end(), [&](std::vector<char>& i) {
        i = std::vector<char>(boardArgs.column, '#');
    });
}

void Board::leftClick(int x, int y) {
    if (!isCoordinateValid(x, y)) throw std::exception("X or Y out of range.");

	if (boardArgs.state != STATE_PLAYING) throw std::exception("haven't start");

    if (boardArgs.board[y][x] == MINE_FLAG) throw std::exception("");

    if (boardArgs.board[y][x] != MINE_MASK && boardArgs.board[y][x] != MINE_SUS)
        throw std::exception("Left click on clicked grid.");

    if (boardArgs.answer[y][x] == MINE_MINE) {
        showMine();
		boardArgs.state = STATE_GAME_OVER;
        boardArgs.status = BOARD_STATUS_LOSE;
        return;
    }

    revealGrid(x, y);

    countBlank();
	if (boardArgs.remainBlankCount == 0) {
		boardArgs.state = STATE_GAME_OVER;
		boardArgs.status = BOARD_STATUS_WIN;
	}
}

void Board::rightClick(int x, int y) {
    if (!isCoordinateValid(x, y)) throw std::exception("X or Y out of range.");

	if (boardArgs.state != STATE_PLAYING) throw std::exception("haven't start");

    if (boardArgs.board[y][x] != MINE_MASK && boardArgs.board[y][x] != MINE_SUS && boardArgs.board[y][x] != MINE_FLAG)
        throw std::exception("Right click on clicked grid.");

    constexpr char symbol[3] = {MINE_MASK, MINE_FLAG, MINE_SUS};

    for (int i = 0; i < 3; i++) {
        if (boardArgs.board[y][x] == symbol[i]) {
            boardArgs.board[y][x] = symbol[((i == 2) ? 0 : (i + 1))];
            if (symbol[i] == MINE_FLAG) boardArgs.flagCount--;
            else boardArgs.flagCount++;
            return;
        }
    }
}

const BoardArgs& Board::getBoardArgs() const {
    return boardArgs;
}

void Board::print(int printOption) const {
	if (printOption == PRINT_STATE) {
		std::string list[3] = { "Standby","Playing","GameOver" };
		hout << list[boardArgs.state];
		return;
	}
	if (boardArgs.row == 0 || boardArgs.column == 0) throw std::exception("Board is not ready yet");
    switch (printOption) {
    case PRINT_BOARD:
        hout << std::endl;
        for (const std::vector<char>& items : boardArgs.board) {
            for (const char& item : items) hout << item << " ";
            hout << std::endl;
        }
        break;
        
    case PRINT_ANSWER:
        hout << std::endl;
        for (const std::vector<char>& items : boardArgs.answer) {
            for (const char& item : items) hout << item << " ";
            hout << std::endl;
        }
        break;
        
    case PRINT_BOMB_COUNT:
        hout << boardArgs.bombCount;
        break;
        
    case PRINT_FLAG_COUNT:
        hout << boardArgs.flagCount;
        break;
        
    case PRINT_OPEN_BLANK:
        hout << boardArgs.openBlankCount;
        break;
        
    case PRINT_REMAIN_BLANK:
        hout << boardArgs.remainBlankCount;
        break;
    default:
        throw std::exception("Invalid use");
    }
}
