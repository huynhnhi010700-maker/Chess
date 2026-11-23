#pragma once
#include <array>
#include <vector>
#include <string>
#include "Piece.hpp"
#include "Move.hpp"
class Board {
public:
    Board();
    Piece get(int r,int c)const;
    void set(int r,int c,Piece p);
    void reset();
    bool inBounds(int r,int c)const;
    std::vector<Move> legalMoves(Piece::Side side)const;
    std::vector<Move> legalFrom(int r,int c)const;
    bool inCheck(Piece::Side side)const;
    bool hasGeneral(Piece::Side side)const;
    bool apply(const Move& m);
    std::string serialize()const;
    bool deserialize(const std::string& data);
private:
    std::array<std::array<Piece,9>,10> cells;
    std::vector<Move> pseudoFrom(int r,int c)const;
    bool isAttacked(int r,int c,Piece::Side by)const;
    bool inPalace(int r,int c,Piece::Side side)const;
};

