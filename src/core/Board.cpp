#include "Board.hpp"
#include "../pieces/Tuong.hpp"
#include "../pieces/Si.hpp"
#include "../pieces/Voi.hpp"
#include "../pieces/Ma.hpp"
#include "../pieces/Xe.hpp"
#include "../pieces/Phao.hpp"
#include "../pieces/Tot.hpp"
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
}
Board::Board(){reset();}
bool Board::inBounds(int r,int c)const{return r>=0&&r<10&&c>=0&&c<9;}
Piece Board::get(int r,int c)const{return inBounds(r,c)?cells[r][c]:Piece();}
void Board::set(int r,int c,Piece p){if(inBounds(r,c))cells[r][c]=p;}
void Board::reset(){
    for(auto& row:cells)for(auto& p:row)p=Piece();
    Piece::Type back[9]={Piece::Rook,Piece::Horse,Piece::Elephant,Piece::Advisor,Piece::General,Piece::Advisor,Piece::Elephant,Piece::Horse,Piece::Rook};
    for(int c=0;c<9;++c){set(0,c,Piece(back[c],Piece::Black));set(9,c,Piece(back[c],Piece::Red));}
    set(2,1,Piece(Piece::Cannon,Piece::Black));set(2,7,Piece(Piece::Cannon,Piece::Black));
    set(7,1,Piece(Piece::Cannon,Piece::Red));set(7,7,Piece(Piece::Cannon,Piece::Red));
    for(int c=0;c<9;c+=2){set(3,c,Piece(Piece::Soldier,Piece::Black));set(6,c,Piece(Piece::Soldier,Piece::Red));}
}
bool Board::inPalace(int r,int c,Piece::Side side)const{return c>=3&&c<=5&&(side==Piece::Red?r>=7&&r<=9:r>=0&&r<=2);}
std::vector<Move> Board::pseudoFrom(int r,int c)const {
 if(!inBounds(r,c))return {};
 Piece p=get(r,c);
 if(p.side==Piece::None)return {};
 if((p.type==Piece::General||p.type==Piece::Advisor)&&!inPalace(r,c,p.side))return {};
 if(p.type==Piece::Elephant&&(p.side==Piece::Red?r<5:r>4))return {};
 // Polymorphism: BanCo khong can biet quy tac tung quan.
 static const Tuong tuong; static const Si si; static const Voi voi;
 static const Ma ma; static const Xe xe; static const Phao phao; static const Tot tot;
 const QuanCo* quan=nullptr;
 switch(p.type){
 case Piece::General:quan=&tuong;break;case Piece::Advisor:quan=&si;break;
 case Piece::Elephant:quan=&voi;break;case Piece::Horse:quan=&ma;break;
 case Piece::Rook:quan=&xe;break;case Piece::Cannon:quan=&phao;break;
 case Piece::Soldier:quan=&tot;break;default:return {};
 }
 return quan->nuocDi(*this,r,c,p.side);
}
bool Board::hasGeneral(Piece::Side side)const{for(int r=0;r<10;++r)for(int c=0;c<9;++c){Piece p=get(r,c);if(p.side==side&&p.type==Piece::General)return true;}return false;}
bool Board::isAttacked(int r,int c,Piece::Side by)const{
    for(int rr=0;rr<10;++rr)for(int cc=0;cc<9;++cc){if(get(rr,cc).side!=by)continue;for(const Move& m:pseudoFrom(rr,cc))if(m.tr==r&&m.tc==c)return true;}
    return false;
}
bool Board::inCheck(Piece::Side side)const{
    for(int r=0;r<10;++r)for(int c=0;c<9;++c){Piece p=get(r,c);if(p.type==Piece::General&&p.side==side)return isAttacked(r,c,opposite(side));}
    return true;
}
std::vector<Move> Board::legalFrom(int r,int c)const{
    std::vector<Move> good;Piece p=get(r,c);if(p.side==Piece::None)return good;
    for(const Move& m:pseudoFrom(r,c)){Board next=*this;next.apply(m);if(!next.inCheck(p.side))good.push_back(m);}
    return good;
}
std::vector<Move> Board::legalMoves(Piece::Side side)const{
    std::vector<Move> good;for(int r=0;r<10;++r)for(int c=0;c<9;++c)if(get(r,c).side==side){auto v=legalFrom(r,c);good.insert(good.end(),v.begin(),v.end());}return good;
}
bool Board::apply(const Move& m){if(!inBounds(m.fr,m.fc)||!inBounds(m.tr,m.tc)||get(m.fr,m.fc).side==Piece::None)return false;set(m.tr,m.tc,get(m.fr,m.fc));set(m.fr,m.fc,Piece());return true;}
std::string Board::serialize()const{std::string s;for(int r=0;r<10;++r)for(int c=0;c<9;++c){Piece p=get(r,c);int code=int(p.type)*int(p.side);s+=std::to_string(code);s+=' ';}return s;}
bool Board::deserialize(const std::string& data){std::istringstream in(data);std::array<std::array<Piece,9>,10> tmp;for(auto& row:tmp)for(auto& p:row){int code;if(!(in>>code)||std::abs(code)>7)return false;p=Piece(Piece::Type(std::abs(code)),code>0?Piece::Red:code<0?Piece::Black:Piece::None);}cells=tmp;return true;}
