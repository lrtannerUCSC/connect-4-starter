#pragma once
#include "Game.h"

// NOTE: If Square class needs modifications to support colored squares for checkerboard pattern,
// add a method like setColor(ImVec4 color) to Square class

class ConnectFour : public Game
{
public:
    ConnectFour();
    ~ConnectFour();

    // Required virtual methods from Game base class
    void        setUpBoard() override;
    Player*     checkForWinner() override;
    bool        checkForDraw() override;
    std::string initialStateString() override;
    std::string stateString() override;
    void        setStateString(const std::string &s) override;
    bool        actionForEmptyHolder(BitHolder &holder) override;
    bool        canBitMoveFrom(Bit &bit, BitHolder &src) override;
    bool        canBitMoveFromTo(Bit &bit, BitHolder &src, BitHolder &dst) override;
    void        stopGame() override;
    void        bitMovedFromTo(Bit &bit, BitHolder &src, BitHolder &dst) override;

    // AI methods
    void        updateAI() override;
    bool        gameHasAI() override { return true; } // Set to true when AI is implemented
    Grid* getGrid() override { return _grid; }

private:
    Bit *       PieceForPlayer(const int playerNumber);
    Player*     ownerAt(int index ) const;
    int         negamax(std::string& state, int depth, int alpha, int beta, int playerColor);

    Grid*       _grid;
};