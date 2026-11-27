#include "ComputerPlayer.hpp"
#include <algorithm>
#include <cmath>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <limits>
#include <sstream>
#include <filesystem>
namespace {
Piece::Side opposite(Piece::Side p){return p==Piece::Red?Piece::Black:Piece::Red;}
int value(Piece::Type t){switch(t){case Piece::General:return 20000;case Piece::Rook:return 900;case Piece::Cannon:return 450;case Piece::Horse:return 420;case Piece::Elephant:return 200;case Piece::Advisor:return 200;case Piece::Soldier:return 100;default:return 0;}}
int moveOrder(const Board& board,const Move& move){
    int score=value(board.get(move.tr,move.tc).type)*10;
    Board next=board;next.apply(move);
    const Piece::Side enemy=opposite(board.get(move.fr,move.fc).side);
    if(!next.hasGeneral(enemy))return score+1000000;
    if(next.inCheck(enemy))score+=20000;
    return score;
}
}
int ComputerPlayer::evaluate(const Board& board,Piece::Side side)const{int score=0;for(int r=0;r<10;++r)for(int c=0;c<9;++c){Piece p=board.get(r,c);if(p.side==Piece::None)continue;int v=value(p.type);if(p.type==Piece::Soldier){int progress=p.side==Piece::Red?9-r:r;v+=progress*12;}score+=(p.side==side?v:-v);}if(board.inCheck(opposite(side)))score+=90;if(board.inCheck(side))score-=120;return score;}
int ComputerPlayer::negamax(const Board& board,Piece::Side side,int depth,int alpha,int beta){if(!board.hasGeneral(side))return -500000-depth;auto options=board.legalMoves(side);if(options.empty())return -500000-depth;if(depth==0)return evaluate(board,side);std::stable_sort(options.begin(),options.end(),[&](const Move& a,const Move& b){return moveOrder(board,a)>moveOrder(board,b);});int best=-1000000;for(const auto& m:options){Board next=board;next.apply(m);int score=-negamax(next,opposite(side),depth-1,-beta,-alpha);best=std::max(best,score);alpha=std::max(alpha,score);if(alpha>=beta)break;}return best;}
Move ComputerPlayer::choose(const Board& board,Piece::Side side,int difficulty){auto opts=board.legalMoves(side);if(opts.empty())return Move(-1,-1,-1,-1);std::stable_sort(opts.begin(),opts.end(),[&](const Move& a,const Move& b){return moveOrder(board,a)>moveOrder(board,b);});if(difficulty==0){
        Move chosen=opts.front();int best=-1000000;
        for(const Move& move:opts){Board next=board;next.apply(move);int score=-evaluate(next,opposite(side));const Piece::Side enemy=opposite(side);if(!next.hasGeneral(enemy)||next.legalMoves(enemy).empty())return move;if(next.inCheck(enemy))score+=1800;if(score>best){best=score;chosen=move;}}
        return chosen;
    }
    int best=-10000000;Move chosen=opts.front();
    // Restrict hard candidates for responsiveness on a single-threaded UI.
    int candidates=difficulty==2?std::min(12,(int)opts.size()):(int)opts.size();
    for(int i=0;i<candidates;++i){const auto& m=opts[i];Board next=board;next.apply(m);
        int score=-negamax(next,opposite(side),difficulty==2?2:1,-1000000,1000000);
        if(score>best){best=score;chosen=m;}}
    return chosen;}
