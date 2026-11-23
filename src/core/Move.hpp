#pragma once
class Move {
public:
    int fr=0,fc=0,tr=0,tc=0;
    Move()=default;
    Move(int a,int b,int c,int d):fr(a),fc(b),tr(c),tc(d){}
    bool same(const Move& m)const{return fr==m.fr&&fc==m.fc&&tr==m.tr&&tc==m.tc;}
};

