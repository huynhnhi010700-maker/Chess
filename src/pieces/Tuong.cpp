#include "Tuong.hpp"
#include "../core/Board.hpp"
#include <cmath>

std::vector<Move> Tuong::nuocDi(const Board& banCo,int r,int c,Piece::Side side) const {
 std::vector<Move> out;
 if(!banCo.inBounds(r,c)||side==Piece::None)return out;
 auto add=[&](int rr,int cc){if(banCo.inBounds(rr,cc)&&banCo.get(rr,cc).side!=side)out.emplace_back(r,c,rr,cc);};
 for (int dr : {-1,1,0,0}) for (int dc : {-1,1,0,0}) {
 if (std::abs(dr)+std::abs(dc)!=1) continue;
 int rr=r+dr, cc=c+dc;
 if(cc>=3&&cc<=5&&(side==Piece::Red?(rr>=7&&rr<=9):(rr>=0&&rr<=2))) add(rr,cc);
}
for(int d:{-1,1}) for(int rr=r+d; banCo.inBounds(rr,c);rr+=d) {
 auto q=banCo.get(rr,c); if(q.side!=Piece::None){if(q.type==Piece::General&&q.side!=side)add(rr,c);break;}
}
 return out;
}
