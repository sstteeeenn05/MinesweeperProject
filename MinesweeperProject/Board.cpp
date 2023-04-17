/*****************************************************************
* File: Board.cpp
 * Author: ´¿±R®¦¡BÃ¹¬ý²M
 * Create Date: 2023/04/06
 * Editor: ´¿±R®¦¡BÃ¹¬ý²M
 * Update Date: 2023/04/17
 * Description: a class to construct the minesweeper game.
 *********************************************************************/

#include "Board.h"
#include <algorithm>
#include <random>
#include <exception>
#include <queue>
#include <fstream>
#include <ctime>

// Intents: to initialize the boards.
// Pre: none
// Post: none
void Board::initializeBoards() {
    // initialize the board
    boardArgs.board = std::vector<std::vector<char>>
        (boardArgs.row, std::vector<char>(boardArgs.column, MINE_MASK));
    // initialize the answer board
    boardArgs.answer = std::vector<std::vector<char>>
        (boardArgs.row, std::vector<char>(boardArgs.column, MINE_NULL));
}

// Intents: to calculate the answer by input file
// Pre: the input file 
// Post: it will throw exception when input data is invalid
void Board::calculateAnswerByInput(std::ifstream& inputFile) {
    // reset the bomb count
    boardArgs.bombCount=0;

    // go through all element in board
    for (int i = 0; i < boardArgs.row; i++) {
        for (int j = 0; j < boardArgs.column; j++) {
            if (inputFile.get() == MINE_MINE) {
                // fill in the mine
                boardArgs.answer[i][j] = MINE_MINE;
                // count mine
                boardArgs.bombCount++;
                //plus one around the mine
                plusOneAroundTheMine(j, i);
            }
        }
        
        // check column count is valid
        if (inputFile.get() != '\n' && !inputFile.eof()) throw std::exception("File format error.");

        // check row count is valid
        if (inputFile.eof() && i != boardArgs.row - 1) throw std::exception("File format error.");
    }
}

// Intents: to generate a board by the random rate
// Pre: none
// Post: none
void Board::randomTheBoard() {
    // reset the bomb count
    boardArgs.bombCount=0;

    // set random device to rand a seed
    std::random_device dev;
    // set the seed
    std::mt19937 generator(dev());
    // set the range of the mine
    const unsigned int mineRange = static_cast<int>(boardArgs.randomRate * 100);

    // go through all element in board
    for (int i = 0; i < boardArgs.row; i++) {
        for (int j = 0; j < boardArgs.column; j++) {
            // if the rand number is within the range
            if (generator() % 100 + 1 <= mineRange) {
                // fill a mine
                boardArgs.answer[i][j] = MINE_MINE;
                // count bomb
                boardArgs.bombCount++;
                //plus one around the mine
                plusOneAroundTheMine(j, i);
            }
        }
    }
}

// Intents: to plus one around the mine
// Pre: the position x and y
// Post: none
void Board::plusOneAroundTheMine(int x, int y) {
    // go through all element in 3*3 area around the input coordinate
    for (int i = y - 1; i <= y + 1; i++) {
        for (int j = x - 1; j <= x + 1; j++) {
            // if it is valid, then +1
            if (isCoordinateValid(j, i) && boardArgs.answer[i][j] != MINE_MINE) {
                boardArgs.answer[i][j]++;
            }
        }
    }
}

// Intents: to count the blank
// Pre: none
// Post: the result will be stored to boardArgs
void Board::countBlank() {
    // reset the blank count
    int tempBlankCount = 0;

    // go through all element in board
    for (int i = 0; i < boardArgs.row; i++) {
        for (int j = 0; j < boardArgs.column; j++) {
            // if board is not masked and not the mine, then remain blank count +1
            if ((boardArgs.board[i][j] == MINE_MASK ||
                 boardArgs.board[i][j] == MINE_FLAG ||
                 boardArgs.board[i][j] == MINE_SUS) &&
                boardArgs.answer[i][j] != MINE_MINE)
                tempBlankCount++;
        }
    }

    // assign to member variable
    boardArgs.remainBlankCount = tempBlankCount;
    // calculate the open blank count by the remain blank count
    boardArgs.openBlankCount = boardArgs.row * boardArgs.column - boardArgs.bombCount - boardArgs.remainBlankCount;
}

// Intents: to check if the coordinate is valid or not
// Pre: the position x and y
// Post: return true if it is valid or false otherwise
bool Board::isCoordinateValid(int x, int y) const {
    // to determine if the coordinate is valid or not
    return (x >= 0 && x < boardArgs.column && y >= 0 && y < boardArgs.row);
}

// Intents: all the mine in game board when step on the mind
// Pre: none
// Post: none
void Board::showMine() {
    // go through all element in the board to show the mine
    for (int i = 0; i < boardArgs.row; i++) {
        for (int j = 0; j < boardArgs.column; j++) {
            // if it is the mine
            if (boardArgs.answer[i][j] == MINE_MINE)
                // show the mine
                boardArgs.board[i][j] = MINE_MINE;
        }
    }
}

// Intents: to reveal nearby grid when left clicked
// Pre: the clicked coordinate x and y
// Post: none
void Board::revealGrid(int x, int y) {
    // local structure Pos
    struct Pos {
        int x;
        int y;
    };

    // a queue the deal with the data
    std::queue<Pos> queue;
    // push the current point into thw queue
    queue.push(Pos{x, y});

    // deal with the data until there is no data left
    while (!queue.empty()) {
        // the data to deal with in this loop
        const Pos currentPos = queue.front();
        // take it out of the queue
        queue.pop();

        // if the symbol on the position is '0' and masked
        // , then deal with 8 grid around it 
        if (boardArgs.answer[currentPos.y][currentPos.x] == MINE_NULL &&
            (boardArgs.board[currentPos.y][currentPos.x] == MINE_MASK
             || boardArgs.board[currentPos.y][currentPos.x] == MINE_SUS)) {
            // push 8 grid around it into the queue 
            for (int i = currentPos.y - 1; i <= currentPos.y + 1; i++) {
                for (int j = currentPos.x - 1; j <= currentPos.x + 1; j++) {
                    // if the coordinate is valid and the symbol is masked
                    // , then push it into the queue
                    if (isCoordinateValid(j, i)) {
                        if ((boardArgs.board[i][j] == MINE_MASK || boardArgs.board[i][j] == MINE_SUS))
                            queue.push(Pos{j, i});
                    }
                }
            }
        }

        // if current position's symbol is not the mine or  the flag, then reveal it.
        if (boardArgs.answer[currentPos.y][currentPos.x] != MINE_MINE && boardArgs.board[currentPos.y][currentPos.x] != MINE_FLAG)
            boardArgs.board[currentPos.y][currentPos.x] = boardArgs.answer[currentPos.y][currentPos.x];
    }
}

// Intents: default constructor
// Pre: none
// Post: none
Board::Board() = default;

// Intents: load the board by file
// Pre: a string of the file path
// Post: it will throw exception when some data is invalid
void Board::load(const char* path) {
    // state check
	if (boardArgs.state != STATE_STANDBY) throw std::exception("not Standby");

    // open input file
    std::ifstream inputFile(path);

    // file check
    if (inputFile.fail()) throw std::exception("File does not exist");

    // input row and column
    inputFile >> boardArgs.row >> boardArgs.column;
    inputFile.get();

    // input row and column check
    if (boardArgs.row <= 0 || boardArgs.column <= 0) {
        inputFile.close();
        throw std::exception("Row or column out of range.");
    }

    // initialize
    boardArgs.mode = MODE_READ_BOARD;
    boardArgs.path = path;
    initializeBoards();

    // generate the board
    calculateAnswerByInput(inputFile);
    inputFile.close();
    // count the remain and open blank
    countBlank();
}

// Intents: load the board by random rate
// Pre: a string of the file path
// Post: it will throw exception when some data is invalid
void Board::load(int inputRow, int inputColumn, double randomRate) {
    // state check
	if (boardArgs.state != STATE_STANDBY) throw std::exception("not Standby");

    // input row and column
    if (inputRow <= 0 || inputColumn <= 0) throw std::exception("Row or column out of range.");

    // input random rate check
    if (randomRate < 0) throw std::exception("Random rate out of range.");

    // initialize
    boardArgs.mode = MODE_INPUT_RATE;
    boardArgs.randomRate = randomRate;
    boardArgs.row = inputRow;
    boardArgs.column = inputColumn;
    initializeBoards();

    // generate the board
    randomTheBoard();
    // count the remain and open blank
    countBlank();
}

// Intents: load the board by random count
// Pre: a string of the file path
// Post: it will throw exception when some data is invalid
void Board::load(int inputRow, int inputColumn, int mineCount) {
    // state check
	if (boardArgs.state != STATE_STANDBY) throw std::exception("not Standby");

    // input row and column check
    if (inputRow <= 0 || inputColumn <= 0) throw std::exception("Row or column out of range.");

    // input mine count check
    if (mineCount > inputRow * inputColumn || mineCount < 0)
        throw std::exception("Mine count out of range.");

    // initialize
    boardArgs.mode = MODE_INPUT_COUNT;
    boardArgs.row = inputRow;
    boardArgs.column = inputColumn;
    boardArgs.bombCount = mineCount;
    initializeBoards();

    // set random device
    std::random_device dev;
    // set seed
    std::mt19937 generator(dev());

    // declare a 1-d vector to save the temp board
    std::vector<char> tempBoard(boardArgs.row * boardArgs.column, MINE_NULL);
    // fill the board with needed mine count
    std::fill_n(tempBoard.begin(), mineCount, MINE_MINE);
    // shuffle the 1-d vector
    std::shuffle(tempBoard.begin(), tempBoard.end(), generator);

    // fill the boardArgs.board with the shuffled 1-d vector
    for (int i = 0; i < boardArgs.row; i++) {
        for (int j = 0; j < boardArgs.column; j++) {
            // if it is mine, then add one around the mine
            if (tempBoard[i * boardArgs.column + j] == MINE_MINE) {
                // fill the position with the mine
                boardArgs.answer[i][j] = MINE_MINE;
                // add one around the mine
                plusOneAroundTheMine(j, i);
            }
        }
    }

    // count the remain and open blank
    countBlank();
}

// Intents: submit the score to the leader board
// Pre: a string of the username and a double of the playing time
// Post: none
void Board::submitScore(const std::string& username, double time) const {
    // make directory if it is not exist
    system("if not exist submit mkdir submit");
    // open the leaderBoard file to output
    std::ofstream leaderBoard("submit/leaderBoard.txt",std::ofstream::app);

    // get the file name by current time
    const std::string currentTime = std::to_string(std::time(nullptr)); 
    const std::string boardFilePath = "submit/" + currentTime + ".txt";
    
    // open the boardFile to output
    std::ofstream boardFile(boardFilePath, std::ofstream::openmode());

    // print record to the leader board
    leaderBoard << currentTime << " " << username << " "<< time << " " 
                << boardArgs.row << " " << boardArgs.column << " " << boardArgs.bombCount << std::endl;

    // print rows and columns to the board file
    boardFile<< boardArgs.row << " " << boardArgs.column << std::endl;

    // print the whole board to the board file
    for (const std::vector<char>& items : boardArgs.answer) {
        for (const char& item : items) {
            if(item == MINE_MINE) boardFile << 'X';
            else boardFile<<'O';
        }
        boardFile << std::endl;
    }

    // close files
    leaderBoard.close();
    boardFile.close();
}

// Intents: to start the game
// Pre: none
// Post: it will throw exception when some data is invalid
void Board::startGame() {
    // load check
	if (boardArgs.row <= 0 || boardArgs.column <= 0) throw std::exception("Board is not load yet.");

    // state check
	if (boardArgs.state != STATE_STANDBY) throw std::exception("wrong state");
    
    // change state to playing
	boardArgs.state = STATE_PLAYING;
}

// Intents: to re-mask the board for the replay feature in gui (normal mode)
// Pre: none
// Post: none
void Board::maskBoard() {
    // reset the status and state
    boardArgs.status = BOARD_STATUS_CONTINUE;
    boardArgs.state = STATE_STANDBY;

    // re-mask the board
     boardArgs.board.assign(boardArgs.row, std::vector<char>(boardArgs.column,'#'));
}

// Intents: to left click the board
// Pre: the click coordinate x and y
// Post: it will throw exception when some data is invalid
void Board::leftClick(int x, int y) {
    // coordinate check
    if (!isCoordinateValid(x, y)) throw std::exception("X or Y out of range.");

    // state check
	if (boardArgs.state != STATE_PLAYING) throw std::exception("haven't start");

    // flag check
    if (boardArgs.board[y][x] == MINE_FLAG) throw std::exception("");

    // mask check
    if (boardArgs.board[y][x] != MINE_MASK && boardArgs.board[y][x] != MINE_SUS)
        throw std::exception("Left click on clicked grid.");

    // if step on the mine, you lose
    if (boardArgs.answer[y][x] == MINE_MINE) {
        // show all the mine
        showMine();
        // change the state to game over
		boardArgs.state = STATE_GAME_OVER;
        // change the status to lose
        boardArgs.status = BOARD_STATUS_LOSE;
        return;
    }

    // reveal grid
    revealGrid(x, y);

    // count open and remain blank
    countBlank();

    // if the remain blank count equals 0, then you win
	if (boardArgs.remainBlankCount == 0) {
	    // change the state to game over
		boardArgs.state = STATE_GAME_OVER;
	    // change the status to win
		boardArgs.status = BOARD_STATUS_WIN;
	}
}

// Intents: to right click the board
// Pre: the click coordinate x and y
// Post: it will throw exception when some data is invalid
void Board::rightClick(int x, int y) {
    // coordinate check
    if (!isCoordinateValid(x, y)) throw std::exception("X or Y out of range.");

    // state check
	if (boardArgs.state != STATE_PLAYING) throw std::exception("haven't start");

    // mask check
    if (boardArgs.board[y][x] != MINE_MASK && boardArgs.board[y][x] != MINE_SUS && boardArgs.board[y][x] != MINE_FLAG)
        throw std::exception("Right click on clicked grid.");

    // table for symbols
    constexpr char symbol[3] = {MINE_MASK, MINE_FLAG, MINE_SUS};

    // to check which symbol fit the symbol on the board
    for (int i = 0; i < 3; i++) {
        // if a symbol fits, then change it to be the next symbol
        if (boardArgs.board[y][x] == symbol[i]) {
            // # -> f -> ? -> # and cycle continues
            boardArgs.board[y][x] = symbol[((i == 2) ? 0 : (i + 1))];
            
            // if the symbol after change is not flag. then the decrease the flag count
            if (symbol[i] == MINE_FLAG) boardArgs.flagCount--;
            // or increase otherwise
            else boardArgs.flagCount++;
            return;
        }
    }
}

// Intents: to get the const reference of the boardArgs 
// Pre: none
// Post: it returns the const reference of the boardArgs 
const BoardArgs& Board::getBoardArgs() const {
    // return the boardArgs
    return boardArgs;
}

// Intents: the print the selected data.
// Pre: the selection
// Post: print the selected data
void Board::print(int printOption) const {
    // if the option is print state, then print the state
	if (printOption == PRINT_STATE) {
	    // thw state list 
		const std::string list[3] = { "Standby","Playing","GameOver" };
	    // use function from handler to handle output(print)
		hout << list[boardArgs.state];
		return;
	}

    // load check
	if (boardArgs.row == 0 || boardArgs.column == 0) {
	    // do not print anything when the board have not load
	    hout << '\0';
	    return;
	}

    // switch all the remain option
    switch (printOption) {
    case PRINT_BOARD:
        // print the game board
        hout << std::endl;
        for (const std::vector<char>& items : boardArgs.board) {
            for (const char& item : items) hout << item << " ";
            hout << std::endl;
        }
        break;
        
    case PRINT_ANSWER:
        // print the answer board
        hout << std::endl;
        for (const std::vector<char>& items : boardArgs.answer) {
            for (const char& item : items) hout << item << " ";
            hout << std::endl;
        }
        break;
        
    case PRINT_BOMB_COUNT:
        // print the bomb count
        hout << boardArgs.bombCount;
        break;
        
    case PRINT_FLAG_COUNT:
        // print the flag count
        hout << boardArgs.flagCount;
        break;
        
    case PRINT_OPEN_BLANK:
        // print the open blank count
        hout << boardArgs.openBlankCount;
        break;
        
    case PRINT_REMAIN_BLANK:
        // print remain blank count
        hout << boardArgs.remainBlankCount;
        break;
    default:
        // invalid command
        throw std::exception("Invalid use");
    }
}
