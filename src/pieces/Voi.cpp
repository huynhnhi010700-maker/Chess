#include "Voi.hpp"
#include "../core/Board.hpp"
#include <cmath>

std::vector<Move> Voi::nuocDi(const Board& banCo,int r,int c,Piece::Side side) const {
 std::vector<Move> out;
 if(!banCo.inBounds(r,c)||side==Piece::None)return out;
 auto add=[&](int rr,int cc){if(banCo.inBounds(rr,cc)&&banCo.get(rr,cc).side!=side)out.emplace_back(r,c,rr,cc);};
 for(int dr:{-2,2})for(int dc:{-2,2}){
 int rr=r+dr,cc=c+dc;
 if(banCo.inBounds(rr,cc)&&(side==Piece::Red?rr>=5:rr<=4)&&banCo.get(r+dr/2,c+dc/2).side==Piece::None)add(rr,cc);
}
 return out;
}
