#include "Xe.hpp"
#include "../core/Board.hpp"
#include <cmath>

std::vector<Move> Xe::nuocDi(const Board& banCo,int r,int c,Piece::Side side) const {
 std::vector<Move> out;
 if(!banCo.inBounds(r,c)||side==Piece::None)return out;
 auto add=[&](int rr,int cc){if(banCo.inBounds(rr,cc)&&banCo.get(rr,cc).side!=side)out.emplace_back(r,c,rr,cc);};
 const int dr[4]={-1,1,0,0},dc[4]={0,0,-1,1};
for(int i=0;i<4;++i)for(int rr=r+dr[i],cc=c+dc[i];banCo.inBounds(rr,cc);rr+=dr[i],cc+=dc[i]){
 auto q=banCo.get(rr,cc);if(q.side==Piece::None)add(rr,cc);
 else {if(q.side!=side)add(rr,cc);break;}
}
 return out;
}
