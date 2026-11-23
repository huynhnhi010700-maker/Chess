#include "Tot.hpp"
#include "../core/Board.hpp"
#include <cmath>

std::vector<Move> Tot::nuocDi(const Board& banCo,int r,int c,Piece::Side side) const {
 std::vector<Move> out;
 if(!banCo.inBounds(r,c)||side==Piece::None)return out;
 auto add=[&](int rr,int cc){if(banCo.inBounds(rr,cc)&&banCo.get(rr,cc).side!=side)out.emplace_back(r,c,rr,cc);};
 add(r+(side==Piece::Red?-1:1),c);
if(side==Piece::Red?r<=4:r>=5){add(r,c-1);add(r,c+1);}
 return out;
}
