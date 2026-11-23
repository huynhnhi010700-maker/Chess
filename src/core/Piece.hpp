#pragma once
class Piece {
public:
    enum Type { Empty=0, General, Advisor, Elephant, Horse, Rook, Cannon, Soldier };
    enum Side { None=0, Red=1, Black=-1 };
    Piece(Type t=Empty, Side s=None):type(t),side(s){}
    Type type;
    Side side;
};

