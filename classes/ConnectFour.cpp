#include "ConnectFour.h"


ConnectFour::ConnectFour()
{
    _grid = new Grid(7, 6);
}

ConnectFour::~ConnectFour()
{
    delete _grid;
}


Bit* ConnectFour::PieceForPlayer(const int playerNumber)
{
    // depending on playerNumber load the "x.png" or the "o.png" graphic
    Bit *bit = new Bit();
    // should possibly be cached from player class?
    bit->LoadTextureFromFile(playerNumber == AI_PLAYER ? "red.png" : "yellow.png");
    bit->setOwner(getPlayerAt(playerNumber == AI_PLAYER ? 1 : 0));
    return bit;
}

void ConnectFour::setUpBoard()
{
    setNumberOfPlayers(2);
    _gameOptions.rowX = 7;
    _gameOptions.rowY = 6;
    _grid->initializeSquares(80, "square.png");

    if (gameHasAI()) {
        setAIPlayer(AI_PLAYER);
    }

    startGame();
}

//
// about the only thing we need to actually fill out for tic-tac-toe
//
bool ConnectFour::actionForEmptyHolder(BitHolder &holder)
{
    if (holder.bit()) {
        return false; // Already occupied
    }
    
    int targetCol = -1;
    int targetRow = -1;
    
    _grid->forEachSquare([&](ChessSquare* square, int col, int row) {
        if (square == &holder) {
            targetCol = col;
            targetRow = row;
        }
    });
    
    if (targetCol == -1) {
        return false;
    }
    
    int lowestEmptyRow = -1;
    for (int row = _gameOptions.rowX - 1; row >= 0; row--) {
        ChessSquare* currentSquare = _grid->getSquare(targetCol, row);
        if (currentSquare && !currentSquare->bit()) {
            lowestEmptyRow = row;
            break;
        }
    }
    
    if (lowestEmptyRow == -1) {
        return false;
    }
    
    Bit *bit = PieceForPlayer(getCurrentPlayer()->playerNumber());
    if (bit) {
        ChessSquare* targetSquare = _grid->getSquare(targetCol, lowestEmptyRow);
        bit->setPosition(targetSquare->getPosition());
        targetSquare->setBit(bit);
        endTurn();
        return true;
    }   
    return false;
}

bool ConnectFour::canBitMoveFrom(Bit &bit, BitHolder &src)
{
    // you can't move anything in tic tac toe
    return false;
}

bool ConnectFour::canBitMoveFromTo(Bit &bit, BitHolder &src, BitHolder &dst)
{
    // you can't move anything in tic tac toe
    return false;
}

void ConnectFour::bitMovedFromTo(Bit &bit, BitHolder &src, BitHolder &dst)
{
    Game::bitMovedFromTo(bit, src, dst);
}

//
// free all the memory used by the game on the heap
//
void ConnectFour::stopGame()
{
    _grid->forEachSquare([](ChessSquare* square, int x, int y) {
        square->destroyBit();
    });
}

//
// helper function for the winner check
//
Player* ConnectFour::ownerAt(int index ) const
{
    auto square = _grid->getSquare(index % 7, index / 7); // This might be backwards
    if (!square || !square->bit()) {
        return nullptr;
    }
    return square->bit()->getOwner();
}

Player* ConnectFour::checkForWinner()
{
    int cols = _gameOptions.rowX = 7;
    int rows = _gameOptions.rowY = 6;
    
    // Check horizontal (4 in a row) - scan rows 0-5, columns 0-3
    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols - 3; col++) {
            Player* player = ownerAt(row * cols + col);
            if (player && 
                player == ownerAt(row * cols + col + 1) && 
                player == ownerAt(row * cols + col + 2) && 
                player == ownerAt(row * cols + col + 3)) {
                return player;
            }
        }
    }
    
    // Check vertical (4 in a column) - scan rows 0-2, columns 0-6
    for (int col = 0; col < cols; col++) {
        for (int row = 0; row < rows - 3; row++) {
            Player* player = ownerAt(row * cols + col);
            if (player && 
                player == ownerAt((row + 1) * cols + col) && 
                player == ownerAt((row + 2) * cols + col) && 
                player == ownerAt((row + 3) * cols + col)) {
                return player;
            }
        }
    }
    
    // Check diagonal (top-left to bottom-right) - scan rows 0-2, columns 0-3
    for (int col = 0; col < cols - 3; col++) {
        for (int row = 0; row < rows - 3; row++) {
            Player* player = ownerAt(row * cols + col);
            if (player && 
                player == ownerAt((row + 1) * cols + (col + 1)) && 
                player == ownerAt((row + 2) * cols + (col + 2)) && 
                player == ownerAt((row + 3) * cols + (col + 3))) {
                return player;
            }
        }
    }
    
    // Check diagonal (top-right to bottom-left) - scan rows 0-2, columns 3-6
    for (int col = 3; col < cols; col++) {
        for (int row = 0; row < rows - 3; row++) {
            Player* player = ownerAt(row * cols + col);
            if (player && 
                player == ownerAt((row + 1) * cols + (col - 1)) && 
                player == ownerAt((row + 2) * cols + (col - 2)) && 
                player == ownerAt((row + 3) * cols + (col - 3))) {
                return player;
            }
        }
    }
    
    return nullptr;
}

bool ConnectFour::checkForDraw()
{
    bool isDraw = true;
    // check to see if the board is full
    _grid->forEachSquare([&isDraw](ChessSquare* square, int x, int y) {
        if (!square->bit()) {
            isDraw = false;
        }
    });
    return isDraw;
}

//
// state strings
//
std::string ConnectFour::initialStateString()
{
    return std::string(7 * 6, '0');
}

//
// this still needs to be tied into imguis init and shutdown
// we will read the state string and store it in each turn object
//
std::string ConnectFour::stateString()
{
    std::string s(7 * 6, '0');
    _grid->forEachSquare([&](ChessSquare* square, int col, int row) {
        Bit *bit = square->bit();
        if (bit) {
            s[row * 7 + col] = std::to_string(bit->getOwner()->playerNumber() + 1)[0];
        }
    });
    return s;
}

//
// this still needs to be tied into imguis init and shutdown
// when the program starts it will load the current game from the imgui ini file and set the game state to the last saved state
//
void ConnectFour::setStateString(const std::string &s)
{
    _grid->forEachSquare([&](ChessSquare* square, int col, int row) {
        int index = row * 7 + col;
        if (index < s.length()) {
            int playerNumber = s[index] - '0';
            if (playerNumber) {
                square->setBit(PieceForPlayer(playerNumber - 1));
            } else {
                square->setBit(nullptr);
            }
        }
    });
}


//
// this is the function that will be called by the AI
//
void ConnectFour::updateAI() 
{
    int bestVal = -1000;
    BitHolder* bestMove = nullptr;
    std::string state = stateString();

    // Only use the column-based approach (remove the forEachSquare part)
    for (int col = 0; col < 7; col++) {  // Fixed: 7 columns, not rowY
        int lowestEmptyRow = -1;
        for (int row = 5; row >= 0; row--) {  // Fixed: rows go from 5 to 0
            int index = row * 7 + col;
            if (state[index] == '0') {
                lowestEmptyRow = row;
                break;
            }
        }
        
        if (lowestEmptyRow != -1) {
            int index = lowestEmptyRow * 7 + col;
            state[index] = '2';  // AI move
            int moveVal = -negamax(state, 0, HUMAN_PLAYER);
            state[index] = '0';
            
            if (moveVal > bestVal) {
                bestMove = _grid->getSquare(col, lowestEmptyRow);
                bestVal = moveVal;
            }
        }
    }

    // Make the best move
    if(bestMove) {
        actionForEmptyHolder(*bestMove);
    }
}

inline bool isAIBoardFullConnectFour(const std::string& state) {
    return state.find('0') == std::string::npos;
}

inline int evaluateAIBoardConnectFour(const std::string& state) {
    const int COLS = 7;
    const int ROWS = 6;
    
    // Check horizontal (4 in a row)
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS - 3; col++) {
            int idx1 = row * COLS + col;
            int idx2 = row * COLS + col + 1;
            int idx3 = row * COLS + col + 2;
            int idx4 = row * COLS + col + 3;
            
            char first = state[idx1];
            if (first != '0' && 
                first == state[idx2] && 
                first == state[idx3] && 
                first == state[idx4]) {
                return 10; // Someone won
            }
        }
    }
    
    // Check vertical (4 in a column)
    for (int col = 0; col < COLS; col++) {
        for (int row = 0; row < ROWS - 3; row++) {
            int idx1 = row * COLS + col;
            int idx2 = (row + 1) * COLS + col;
            int idx3 = (row + 2) * COLS + col;
            int idx4 = (row + 3) * COLS + col;
            
            char first = state[idx1];
            if (first != '0' && 
                first == state[idx2] && 
                first == state[idx3] && 
                first == state[idx4]) {
                return 10; // Someone won
            }
        }
    }
    
    // Check diagonal (top-left to bottom-right)
    for (int col = 0; col < COLS - 3; col++) {
        for (int row = 0; row < ROWS - 3; row++) {
            int idx1 = row * COLS + col;
            int idx2 = (row + 1) * COLS + col + 1;
            int idx3 = (row + 2) * COLS + col + 2;
            int idx4 = (row + 3) * COLS + col + 3;
            
            char first = state[idx1];
            if (first != '0' && 
                first == state[idx2] && 
                first == state[idx3] && 
                first == state[idx4]) {
                return 10; // Someone won
            }
        }
    }
    
    // Check diagonal (top-right to bottom-left)
    for (int col = 3; col < COLS; col++) {
        for (int row = 0; row < ROWS - 3; row++) {
            int idx1 = row * COLS + col;
            int idx2 = (row + 1) * COLS + col - 1;
            int idx3 = (row + 2) * COLS + col - 2;
            int idx4 = (row + 3) * COLS + col - 3;
            
            char first = state[idx1];
            if (first != '0' && 
                first == state[idx2] && 
                first == state[idx3] && 
                first == state[idx4]) {
                return 10; // Someone won
            }
        }
    }
    
    return 0; // No winner
}

//
// player is the current player's number (AI or human)
//
int ConnectFour::negamax(std::string& state, int depth, int playerColor) 
{

    if (depth >= 6) {
        return 0;
    }

    int score = evaluateAIBoardConnectFour(state);

    // Check if AI wins, human wins, or draw
    if(score) { 
        // A winning state is a loss for the player whose turn it is.
        // The previous player made the winning move.
        return -score; 
    }

    if(isAIBoardFullConnectFour(state)) {
        return 0; // Draw
    }

    int bestVal = -1000;
    
    // Try each column for possible moves
    for (int col = 0; col < 7; col++) {
        // Find the lowest empty row in this column
        int lowestEmptyRow = -1;
        for (int row = 5; row >= 0; row--) {
            int index = row * 7 + col;
            if (state[index] == '0') {
                lowestEmptyRow = row;
                break;
            }
        }
        
        // If valid move found in this column
        if (lowestEmptyRow != -1) {
            int index = lowestEmptyRow * 7 + col;
            
            // Make the move - '1' for human, '2' for AI
            state[index] = (playerColor == HUMAN_PLAYER) ? '1' : '2';
            
            // Recursive call with opposite player
            int moveVal = -negamax(state, depth + 1, -playerColor);
            bestVal = std::max(bestVal, moveVal);
            
            // Undo the move
            state[index] = '0';
        }
    }

    return bestVal;
}