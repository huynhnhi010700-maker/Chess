#include "Si.hpp"
#include "../core/Board.hpp"
#include <cmath>

std::vector<Move> Si::nuocDi(const Board& banCo,int r,int c,Piece::Side side) const {
 std::vector<Move> out;
 if(!banCo.inBounds(r,c)||side==Piece::None)return out;
 auto add=[&](int rr,int cc){if(banCo.inBounds(rr,cc)&&banCo.get(rr,cc).side!=side)out.emplace_back(r,c,rr,cc);};
 for(int dr:{-1,1})for(int dc:{-1,1}){
 int rr=r+dr,cc=c+dc;
 if(cc>=3&&cc<=5&&(side==Piece::Red?(rr>=7&&rr<=9):(rr>=0&&rr<=2)))add(rr,cc);
}
 return out;
}
