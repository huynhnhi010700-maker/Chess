#include "Game.hpp"
#include "Paths.hpp"
#include <algorithm>
#include <cmath>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <limits>
#include <map>
#include <sstream>
#include <filesystem>
namespace {
Piece::Side opposite(Piece::Side p){return p==Piece::Red?Piece::Black:Piece::Red;}
int value(Piece::Type t){switch(t){case Piece::General:return 20000;case Piece::Rook:return 900;case Piece::Cannon:return 450;case Piece::Horse:return 420;case Piece::Elephant:return 200;case Piece::Advisor:return 200;case Piece::Soldier:return 100;default:return 0;}}
}
void Game::start(Mode m,float seconds){board.reset();turn=Piece::Red;mode=m;state=Playing;redTime=seconds;blackTime=seconds;log.clear();moves.clear();hasLast=false;}
bool Game::isThreefoldRepetition()const{
    Board position;position.reset();
    Piece::Side side=Piece::Red;
    std::map<std::string,int> counts;
    auto key=[&](){return position.serialize()+std::to_string(int(side));};
    ++counts[key()];
    for(const Move& move:moves){
        if(!position.apply(move))return false;
        side=opposite(side);
        if(++counts[key()]>=3)return true;
    }
    return false;
}
bool Game::hasMatingMaterial(Piece::Side side)const{
    for(int r=0;r<10;++r)for(int c=0;c<9;++c){
        const Piece p=board.get(r,c);
        if(p.side!=side)continue;
        if(p.type==Piece::Rook||p.type==Piece::Cannon||p.type==Piece::Horse||p.type==Piece::Soldier)return true;
    }
    return false;
}
std::string Game::notation(const Move& m,const Piece& p,const Piece& taken){
    // Xiangqi algebraic: piece + origin file (1..9) + direction + destination.
    // Red counts files from the right; Black from the left, seen from its own side.
    const char* names[]={"?","K","A","E","H","R","P","S"};
    int from=p.side==Piece::Red?9-m.fc:m.fc+1;
    int to=p.side==Piece::Red?9-m.tc:m.tc+1;
    bool lateral=m.fr==m.tr;
    bool forward=p.side==Piece::Red?m.tr<m.fr:m.tr>m.fr;
    char direction=lateral?'-':(forward?'+':'-');
    int target=(p.type==Piece::Horse||p.type==Piece::Elephant||p.type==Piece::Advisor)?to:(lateral?to:std::abs(m.tr-m.fr));
    std::ostringstream out;out<<names[p.type]<<from<<direction<<target;
    if(taken.side!=Piece::None)out<<"x";
    return out.str();
}
std::string Game::stateName(State s){switch(s){case Playing:return "Dang choi";case RedWins:return "Do thang";case BlackWins:return "Den thang";case RedTimeout:return "Do het gio - Den thang";case BlackTimeout:return "Den het gio - Do thang";case Draw:return "Hoa";}return "";}
bool Game::makeMove(const Move& m){if(state!=Playing)return false;auto legal=board.legalFrom(m.fr,m.fc);if(board.get(m.fr,m.fc).side!=turn||std::none_of(legal.begin(),legal.end(),[&](const Move& x){return x.same(m);}))return false;
    Piece moving=board.get(m.fr,m.fc),captured=board.get(m.tr,m.tc);board.apply(m);moves.push_back(m);log.push_back(notation(m,moving,captured));lastMove=m;hasLast=true;redTime=120.f;blackTime=120.f;
    Piece::Side other=opposite(turn);if(!board.hasGeneral(other)||board.legalMoves(other).empty())state=turn==Piece::Red?RedWins:BlackWins;
    turn=other;
    if(state==Playing&&((!hasMatingMaterial(Piece::Red)&&!hasMatingMaterial(Piece::Black))||isThreefoldRepetition()))state=Draw;
    return true;
}
void Game::tick(float seconds){
    if(state!=Playing||seconds<=0.f)return;
    float& time=turn==Piece::Red?redTime:blackTime;
    time=std::max(0.f,time-seconds);
    if(time<=0.f){
        turn=opposite(turn);
        float& nextTime=turn==Piece::Red?redTime:blackTime;
        nextTime=120.f;
    }
}
bool Game::save(const std::string& path)const{std::ofstream out(path);if(!out)return false;out<<"XIANGQI_OOP_V2\n"<<int(mode)<<' '<<int(state)<<' '<<int(turn)<<' '<<redTime<<' '<<blackTime<<'\n'<<std::quoted(redName)<<' '<<std::quoted(blackName)<<' '<<std::quoted(saveLabel)<<'\n'<<board.serialize()<<'\n'<<moves.size()<<'\n';for(size_t i=0;i<moves.size();++i)out<<moves[i].fr<<' '<<moves[i].fc<<' '<<moves[i].tr<<' '<<moves[i].tc<<' '<<std::quoted(log[i])<<'\n';return bool(out);}
bool Game::load(const std::string& path){std::ifstream in(path);if(!in)return false;std::string header,boardLine;std::getline(in,header);if(header!="XIANGQI_OOP_V1"&&header!="XIANGQI_OOP_V2")return false;int mo,st,tu;if(!(in>>mo>>st>>tu>>redTime>>blackTime)||mo<0||mo>1||st<0||st>5||(tu!=1&&tu!=-1))return false;mode=Mode(mo);in.ignore(std::numeric_limits<std::streamsize>::max(),'\n');if(header=="XIANGQI_OOP_V2"){if(!(in>>std::quoted(redName)>>std::quoted(blackName)>>std::quoted(saveLabel)))return false;in.ignore(std::numeric_limits<std::streamsize>::max(),'\n');}else{redName="Nguoi choi 1";blackName=mode==HumanVsAI?"May":"Nguoi choi 2";saveLabel="Van cu";}std::getline(in,boardLine);if(!board.deserialize(boardLine))return false;mode=Mode(mo);state=State(st);turn=Piece::Side(tu);size_t count;if(!(in>>count)||count>1000)return false;log.clear();moves.clear();for(size_t i=0;i<count;++i){Move m;std::string note;if(!(in>>m.fr>>m.fc>>m.tr>>m.tc>>std::quoted(note)))return false;moves.push_back(m);log.push_back(note);}if(redTime<=30.f&&blackTime<=30.f)redTime=blackTime=120.f;hasLast=!moves.empty();if(hasLast)lastMove=moves.back();return true;}
bool Game::saveHistory()const{namespace fs=std::filesystem;std::error_code ec;fs::create_directories(saveDirectory() / "history",ec);std::time_t now=std::time(nullptr);std::tm stamp{};
#ifdef _WIN32
localtime_s(&stamp,&now);
#else
localtime_r(&now,&stamp);
#endif
std::ostringstream name;name<<"game_"<<std::put_time(&stamp,"%Y%m%d_%H%M%S")<<"_"<<moves.size()<<".txt";return save((saveDirectory() / "history" / name.str()).string());}
