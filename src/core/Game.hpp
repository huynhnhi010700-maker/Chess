#pragma once
#include "Board.hpp"
#include <string>
#include <vector>
class Game {
public:
    enum Mode { HumanVsHuman, HumanVsAI };
    enum State { Playing, RedWins, BlackWins, RedTimeout, BlackTimeout, Draw };
    std::string redName="Nguoi choi 1",blackName="Nguoi choi 2",saveLabel="Van moi";
    Board board;
    Piece::Side turn=Piece::Red;
    Mode mode=HumanVsHuman;
    State state=Playing;
    float redTime=600,blackTime=600;
    std::vector<std::string> log;
    std::vector<Move> moves;
    Move lastMove;
    bool hasLast=false;
    void start(Mode m,float seconds);
    bool makeMove(const Move& m);
    void tick(float seconds);
    bool save(const std::string& path)const;
    bool load(const std::string& path);
    bool saveHistory()const;
    static std::string notation(const Move& m,const Piece& p,const Piece& captured);
    static std::string stateName(State s);
private:
    bool isThreefoldRepetition()const;
    bool hasMatingMaterial(Piece::Side side)const;
};

