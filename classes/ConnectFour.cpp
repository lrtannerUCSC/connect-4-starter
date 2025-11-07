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
    for (int row = _gameOptions.rowY - 1; row >= 0; row--) {
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
    auto square = _grid->getSquare(index % 7, index / 7);
    if (!square || !square->bit()) {
        return nullptr;
    }
    return square->bit()->getOwner();
}

Player* ConnectFour::checkForWinner()
{
    int cols = 7;
    int rows = 6;
    
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
    // We will use the above negamax with a depth limit.
    std::string state = stateString();
    int bestVal = -1000000;
    int bestCol = -1;
    int COLS = 7;
    int ROWS = 6;

    // Try columns; prefer center when values tie
    const int order[7] = {3, 4, 2, 5, 1, 6, 0};

    for (int oi = 0; oi < COLS; ++oi) {
        int col = order[oi];

        int lowestEmptyRow = -1;
        for (int row = ROWS - 1; row >= 0; --row) {
            int idx = row*COLS + col;
            if (state[idx] == '0') { lowestEmptyRow = row; break; }
        }
        if (lowestEmptyRow == -1) continue;

        int idx = lowestEmptyRow*COLS + col;
        // simulate AI placing '2'
        state[idx] = '2';

        // now it's human to move -> player = -1 (human). negate returned value to AI perspective.
        int moveVal = -negamax(state, 0, -1000000, 1000000, -1);

        // undo
        state[idx] = '0';

        if (moveVal > bestVal) {
            bestVal = moveVal;
            bestCol = col;
        }
    }

    if (bestCol != -1) {
        // actually make the move on the game grid (drop to lowest empty)
        for (int row = ROWS - 1; row >= 0; --row) {
            ChessSquare* square = _grid->getSquare(bestCol, row);
            if (square && !square->bit()) {
                actionForEmptyHolder(*square);
                break;
            }
        }
    }
}

inline bool isAIBoardFullConnectFour(const std::string& state) {
    return state.find('0') == std::string::npos;
}

// ---------- helper to check wins for a specific char -------------------
static bool checkFourInARowForPlayer(const std::string &state, char p)
{
    const int COLS = 7;
    const int ROWS = 6;

    // horizontal
    for (int r = 0; r < ROWS; ++r) {
        for (int c = 0; c < COLS - 3; ++c) {
            int i = r*COLS + c;
            if (state[i] == p && state[i+1] == p && state[i+2] == p && state[i+3] == p) return true;
        }
    }
    // vertical
    for (int c = 0; c < COLS; ++c) {
        for (int r = 0; r < ROWS - 3; ++r) {
            int i = r*COLS + c;
            if (state[i] == p && state[i+COLS] == p && state[i+2*COLS] == p && state[i+3*COLS] == p) return true;
        }
    }
    // diag TL->BR
    for (int r = 0; r < ROWS - 3; ++r) {
        for (int c = 0; c < COLS - 3; ++c) {
            int i = r*COLS + c;
            if (state[i] == p && state[i+COLS+1] == p && state[i+2*(COLS+1)] == p && state[i+3*(COLS+1)] == p) return true;
        }
    }
    // diag TR->BL
    for (int r = 0; r < ROWS - 3; ++r) {
        for (int c = 3; c < COLS; ++c) {
            int i = r*COLS + c;
            if (state[i] == p && state[i+COLS-1] == p && state[i+2*(COLS-1)] == p && state[i+3*(COLS-1)] == p) return true;
        }
    }
    return false;
}

int ConnectFour::evaluateAIBoardConnectFour(const std::string& state) {
    const int COLS = 7;
    const int ROWS = 6;
    
    // Terminal states
    if (checkFourInARowForPlayer(state, '2')) {
        return 100000; // AI win
    }
    if (checkFourInARowForPlayer(state, '1')) {
        return -100000; // Human win
    }
    
    // Pattern-based scoring
    int score = 0;
    
    // Check all possible segments and score partial patterns
    for (int r = 0; r < ROWS; ++r) {
        for (int c = 0; c < COLS; ++c) {

            score += evaluatePosition(state, r, c);
        }
    }
    
    return score;
}

// Helper function to evaluate patterns at a specific position
int ConnectFour::evaluatePosition(const std::string& state, int row, int col) {
    const int COLS = 7;
    const int ROWS = 6;
    int score = 0;
    
    // Check horizontal right
    if (col <= COLS - 4) {
        score += evaluateSegment(state, row, col, 0, 1);
    }
    
    // Check vertical down
    if (row <= ROWS - 4) {
        score += evaluateSegment(state, row, col, 1, 0);
    }
    
    // Check diagonal down-right
    if (row <= ROWS - 4 && col <= COLS - 4) {
        score += evaluateSegment(state, row, col, 1, 1);
    }
    
    // Check diagonal down-left
    if (row <= ROWS - 4 && col >= 3) {
        score += evaluateSegment(state, row, col, 1, -1);
    }
    
    return score;
}

// Evaluate a 4-cell segment and return its score
int ConnectFour::evaluateSegment(const std::string& state, int startRow, int startCol, int rowDelta, int colDelta) {
    int aiCount = 0;
    int humanCount = 0;
    int emptyCount = 0;
    
    for (int i = 0; i < 4; i++) {
        int r = startRow + i * rowDelta;
        int c = startCol + i * colDelta;
        int idx = r * 7 + c;
        
        if (state[idx] == '2') {
            aiCount++;
        } else if (state[idx] == '1') {
            humanCount++;
        } else {
            emptyCount++;
        }
    }

    if (aiCount > 0 && humanCount > 0) {
        return 0;
    }
    
    if (aiCount > 0) {
        if (aiCount == 2 && emptyCount == 2) return 10;
        if (aiCount == 3 && emptyCount == 1) return 100;
    }
    
    if (humanCount > 0) {
        if (humanCount == 2 && emptyCount == 2) return -10;
        if (humanCount == 3 && emptyCount == 1) return -100;
    }
    
    return 0;
}


int ConnectFour::negamax(std::string& state, int depth, int alpha, int beta, int player)
{
    // player: +1 = AI to move, -1 = human to move

    int eval = evaluateAIBoardConnectFour(state);
    if (eval != 0) {
        return (player == 1) ? eval - depth : -eval + depth;
    }

    if (isAIBoardFullConnectFour(state)) {
        return 0; // draw
    }

    if (depth >= 10) {
        return 0;
    }

    int best = -1000000;

    const int COLS = 7;
    const int ROWS = 6;

    const int order[7] = {3, 4, 2, 5, 1, 6, 0};

    for (int oi = 0; oi < COLS; ++oi) {
        int col = order[oi];

        // find lowest empty row in column
        int lowestEmptyRow = -1;
        for (int row = ROWS - 1; row >= 0; --row) {
            int idx = row*COLS + col;
            if (state[idx] == '0') {
                lowestEmptyRow = row;
                break;
            }
        }
        if (lowestEmptyRow == -1) continue;

        int idx = lowestEmptyRow*COLS + col;
        // place piece depending on player
        state[idx] = (player == 1) ? '2' : '1';

        // recursively search: flip player and negate score
        int val = -negamax(state, depth + 1, -beta, -alpha, -player);

        // undo
        state[idx] = '0';

        if (val > best) best = val;
        if (val > alpha) alpha = val;
        if (alpha >= beta) {
            break; // beta cutoff
        }
    }

    return best;
}