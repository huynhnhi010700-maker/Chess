#include "Ma.hpp"
#include "../core/Board.hpp"
#include <cmath>

std::vector<Move> Ma::nuocDi(const Board& banCo,int r,int c,Piece::Side side) const {
 std::vector<Move> out;
 if(!banCo.inBounds(r,c)||side==Piece::None)return out;
 auto add=[&](int rr,int cc){if(banCo.inBounds(rr,cc)&&banCo.get(rr,cc).side!=side)out.emplace_back(r,c,rr,cc);};
 const int dr[4]={-1,1,0,0},dc[4]={0,0,-1,1};
for(int i=0;i<4;++i){int legR=r+dr[i],legC=c+dc[i];
 if(!banCo.inBounds(legR,legC)||banCo.get(legR,legC).side!=Piece::None)continue;
 if(i<2){add(r+dr[i]*2,c-1);add(r+dr[i]*2,c+1);}
 else {add(r-1,c+dc[i]*2);add(r+1,c+dc[i]*2);}
}
 return out;
}
