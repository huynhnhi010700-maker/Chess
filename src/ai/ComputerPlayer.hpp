#pragma once
#include "../core/Board.hpp"
class ComputerPlayer {
public:
    Move choose(const Board& board,Piece::Side side,int difficulty=1);
private:
    int evaluate(const Board& board,Piece::Side side)const;
    int negamax(const Board& board,Piece::Side side,int depth,int alpha,int beta);
};
