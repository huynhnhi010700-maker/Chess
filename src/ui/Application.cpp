#include "Application.hpp"
#include "../core/Game.hpp"
#include "../core/Paths.hpp"
#include "../ai/ComputerPlayer.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <vector>
#include <ctime>
#include <iomanip>
#include <cwctype>

class Application {
public:
    Application():window(sf::VideoMode(1250,940),"CO TUONG OOP - XIANGQI",sf::Style::Titlebar|sf::Style::Close){
        window.setFramerateLimit(60);window.setVerticalSyncEnabled(true);
        if(!(font.loadFromFile("assets/VT323-Regular.ttf") || font.loadFromFile("assets/font/VT323-Regular.ttf") || font.loadFromFile("C:/Windows/Fonts/consola.ttf") || font.loadFromFile("C:/Windows/Fonts/arial.ttf")))std::cerr<<"Khong tai duoc font!\n";
        if(!boardImage.loadFromFile("assets/board.png"))std::cerr<<"Khong tai duoc ban co!\n";
        const std::string files[2][7]={{"","red_general.png","red_advisor.png","red_elephant.png","red_horse.png","red_rook.png","red_cannon.png"},{"","black_general.png","black_advisor.png","black_elephant.png","black_horse.png","black_rook.png","black_cannon.png"}};
        for(int i=0;i<2;++i)for(int j=1;j<=6;++j)pieces[i][j].loadFromFile("assets/"+files[i][j]);
        pieces[0][7].loadFromFile("assets/red_soldier.png");pieces[1][7].loadFromFile("assets/black_soldier.png");
        clickBuffer.loadFromFile("assets/move.wav");captureBuffer.loadFromFile("assets/capture.wav");sound.setVolume(55);
        landscape.loadFromFile("assets/autumn_pixel.png");landscape.setSmooth(false);
        bgMusic.openFromFile("assets/ambient.wav");bgMusic.setLoop(true);bgMusic.setVolume(20);
        loadSettings();updateAudio();
        game.start(Game::HumanVsHuman,120);
    }
    void run(){sf::Clock delta;while(window.isOpen()){
        float dt=std::min(delta.restart().asSeconds(),.1f);
        sf::Event event;while(window.pollEvent(event)){if(event.type==sf::Event::Closed)window.close();else if(event.type==sf::Event::MouseMoved)mousePos={event.mouseMove.x,event.mouseMove.y};else if(event.type==sf::Event::TextEntered && screen==Setup)typeName(event.text.unicode);else if(event.type==sf::Event::KeyPressed)key(event.key.code);else if(event.type==sf::Event::MouseButtonPressed&&event.mouseButton.button==sf::Mouse::Left)click(event.mouseButton.x,event.mouseButton.y);}
        if(screen==Result)resultElapsed+=dt;
        if(screen==Match&&game.state==Game::Playing){
            const Piece::Side turnBeforeTick=game.turn;
            game.tick(dt);
            if(game.turn!=turnBeforeTick){resetSelection();botDelayClock.restart();}
            if(game.mode==Game::HumanVsAI&&game.turn==Piece::Black&&botDelayClock.getElapsedTime().asSeconds()>=0.65f){Move m=ai.choose(game.board,Piece::Black,difficulty);make(m);}
        }
        window.clear(sf::Color(18,28,39));if(screen==Startup)drawStartup();else if(screen==Menu)drawMenu();else if(screen==Help)drawHelp();else if(screen==History)drawHistory();else if(screen==Settings)drawSettings();else if(screen==Result)drawResult();else if(screen==Setup)drawSetup();else drawMatch();window.display();
    }}
private:
    enum Screen { Startup, Menu, Match, Help, History, Settings, Result, Setup };
    sf::RenderWindow window;
    sf::Font font;
    sf::Texture boardImage,landscape;
    sf::Music bgMusic;
    bool english=false,musicEnabled=true,effectsEnabled=true;
    int difficulty=1,menuMode=0,selectedSave=0,editField=0;
    std::wstring entry[3]={L"Nguoi choi 1",L"Nguoi choi 2",L"Van co 1"};
    sf::Vector2i mousePos;
    sf::Texture pieces[2][8];
    sf::SoundBuffer clickBuffer,captureBuffer;
    sf::Sound sound;
    Game game;
    ComputerPlayer ai;
    Screen screen=Startup;
    int cursorR=9,cursorC=4,selR=-1,selC=-1;
    std::vector<Move> options;
    bool savedEnd=false;
    float resultElapsed=0.f;
    sf::Clock noticeClock;
    // Let the red move render before the computer responds.
    sf::Clock botDelayClock;
    std::wstring saveNotice;
    std::wstring setupError;
    sf::Color noticeColor=sf::Color::White;
    std::vector<std::string> historyFiles;
    int historyPage=0;
    std::string namedSlot;
    const float bx=24,by=18,ox=80,oy=90,step=80;

    void text(const std::wstring& s,float x,float y,unsigned size,sf::Color color=sf::Color(36,20,9),bool centered=false){
        // Render in a small bitmap-like grid; nearest neighbor scaling avoids blurred glyphs.
        size+=7;
        unsigned logical=std::max(9u,unsigned(size*.67f));float scale=float(size)/float(logical);
        sf::Text t(s,font,logical);t.setScale(scale,scale);t.setFillColor(color);t.setPosition(std::round(x),std::round(y));
        if(centered){auto b=t.getLocalBounds();t.setOrigin(b.left+b.width/2.f,0);}window.draw(t);
    }
    void centeredText(const std::wstring& s,float cx,float cy,unsigned size,sf::Color color=sf::Color::White){
        size+=7;
        unsigned logical=std::max(9u,unsigned(size*.67f));float scale=float(size)/float(logical);
        sf::Text t(s,font,logical);t.setScale(scale,scale);t.setFillColor(color);
        const auto bounds=t.getLocalBounds();t.setOrigin(bounds.left+bounds.width/2.f,bounds.top+bounds.height/2.f);
        t.setPosition(std::round(cx),std::round(cy));window.draw(t);
    }
    void rect(float x,float y,float w,float h,sf::Color fill,float radius=0){(void)radius;sf::RectangleShape r(sf::Vector2f(w,h));r.setPosition(x,y);r.setFillColor(fill);window.draw(r);}
    void button(const std::wstring& label,float x,float y,float w,float h,bool active=false,bool enableHover=true){
        bool over=enableHover&&hit(mousePos.x,mousePos.y,x,y,w,h);
        rect(x+3,y+4,w,h,sf::Color(79,48,24,150));
        rect(x,y,w,h,active?sf::Color(223,178,120):over?sf::Color(217,170,112):sf::Color(200,150,90));
        const sf::Color border=active?sf::Color(122,32,24):sf::Color(110,64,30);
        const float thickness=active?5.f:3.f;
        rect(x,y,w,thickness,border);rect(x,y+h-thickness,w,thickness,border);
        rect(x,y,thickness,h,border);rect(x+w-thickness,y,thickness,h,border);
        centeredText(label,x+w/2,y+h/2.f,19,sf::Color(36,20,9));
    }
    void woodButton(const std::wstring& label,float x,float y,float w,float h){
        const bool over=hit(mousePos.x,mousePos.y,x,y,w,h);
        rect(x+4,y+5,w,h,sf::Color(79,48,24,150));
        rect(x,y,w,h,over?sf::Color(217,170,112):sf::Color(200,150,90));
        rect(x,y,w,3,sf::Color(110,64,30));
        rect(x,y+h-3,w,3,sf::Color(110,64,30));
        rect(x,y,3,h,sf::Color(110,64,30));rect(x+w-3,y,3,h,sf::Color(110,64,30));
        centeredText(label,x+w/2,y+h/2.f,19,sf::Color(36,20,9));
    }
    void coloredButton(const std::wstring& label,float x,float y,float w,float h,sf::Color base){
        const bool over=hit(mousePos.x,mousePos.y,x,y,w,h);
        (void)base;
        rect(x+5,y+6,w,h,sf::Color(79,48,24,150));
        sf::Color fill=over?sf::Color(185,58,48):sf::Color(166,44,33);
        rect(x,y,w,h,fill);
        rect(x,y,w,4,sf::Color(92,22,15));rect(x,y+h-4,w,4,sf::Color(92,22,15));
        rect(x,y,4,h,sf::Color(92,22,15));rect(x+w-4,y,4,h,sf::Color(92,22,15));
        centeredText(label,x+w/2,y+h/2.f,20,sf::Color(247,232,206));
    }
    bool hit(int mx,int my,float x,float y,float w,float h){return mx>=x&&my>=y&&mx<x+w&&my<y+h;}
    void notify(const std::wstring& message,bool ok=true){saveNotice=message;noticeColor=ok?sf::Color(125,235,158):sf::Color(255,135,110);noticeClock.restart();std::cout<<(ok?"[SAVE OK] ":"[SAVE ERROR] ")<<utf8(message)<<"\n";}
    // Only the player may create a resumable save (F5 or the Save button).
    // Manual saves are isolated from old autosaves and completed-game archives.
    std::filesystem::path manualDirectory() const { return saveDirectory() / "manual"; }
    bool saveCurrent(){
        if(game.state!=Game::Playing){
            notify(tr(L"VÁN ĐÃ KẾT THÚC - KHÔNG THỂ LƯU",L"FINISHED GAME CANNOT BE SAVED"),false);
            return false;
        }
        std::error_code ec;
        std::filesystem::create_directories(manualDirectory(),ec);
        if(ec){notify(tr(L"KHÔNG TẠO ĐƯỢC THƯ MỤC LƯU",L"CANNOT CREATE SAVE FOLDER"),false);return false;}
        if(namedSlot.empty()){
            // Guard against accidentally overwriting another manually saved game.
            if(nameAlreadySaved(wide(game.saveLabel))){
                notify(tr(L"TÊN VÁN BỊ TRÙNG - KHÔNG GHI ĐÈ",L"DUPLICATE NAME - SAVE CANCELLED"),false);
                return false;
            }
            std::time_t stamp=std::time(nullptr);std::tm tm{};
#ifdef _WIN32
            localtime_s(&tm,&stamp);
#else
            localtime_r(&stamp,&tm);
#endif
            std::ostringstream suffix;
            suffix<<std::put_time(&tm,"%Y%m%d_%H%M%S");
            const std::string base=safeStem(game.saveLabel)+"_"+suffix.str();
            namedSlot=base+".txt";
            for(int serial=2;std::filesystem::exists(manualDirectory()/namedSlot);++serial)
                namedSlot=base+"_"+std::to_string(serial)+".txt";
        }
        const auto destination=manualDirectory()/std::filesystem::path(namedSlot).filename();
        const bool ok=game.save(destination.string());
        notify(ok?tr(L"ĐÃ LƯU VÁN THÀNH CÔNG",L"GAME SAVED SUCCESSFULLY"):
                  tr(L"LƯU THẤT BẠI - KIỂM TRA QUYỀN GHI",L"SAVE FAILED - CHECK FILE PERMISSIONS"),ok);
        std::cout<<"Manual save: "<<destination.string()<<" -> "<<(ok?"OK":"FAILED")<<"\n";
        return ok;
    }
    // Finished games are NOT resumable. Remove their former live checkpoint.
    void removeFinishedCheckpoint(){
        if(namedSlot.empty())return;
        std::error_code ec;
        std::filesystem::remove(manualDirectory()/std::filesystem::path(namedSlot).filename(),ec);
        if(ec)std::cerr<<"Could not remove finished save: "<<ec.message()<<"\n";
        namedSlot.clear();
    }
    std::vector<std::string> availableManualSaves() const {
        std::vector<std::filesystem::path> paths;
        std::error_code ec;
        if(!std::filesystem::exists(manualDirectory(),ec))return {};
        for(std::filesystem::directory_iterator it(manualDirectory(),ec),end;!ec&&it!=end;it.increment(ec)){
            if(!it->is_regular_file()||it->path().extension()!=".txt")continue;
            Game preview;
            if(preview.load(it->path().string())&&preview.state==Game::Playing)
                paths.push_back(it->path());
        }
        std::sort(paths.begin(),paths.end(),[](const auto& a,const auto& b){
            std::error_code ea,eb;
            auto ta=std::filesystem::last_write_time(a,ea),tb=std::filesystem::last_write_time(b,eb);
            return !ea&&!eb?ta>tb:a.filename().string()>b.filename().string();
        });
        std::vector<std::string> out;
        for(const auto& path:paths)out.push_back(path.string());
        return out;
    }
    static std::string utf8(const std::wstring& x){sf::String value(x);auto bytes=value.toUtf8();return std::string(bytes.begin(),bytes.end());}
    static std::wstring wide(const std::string& x){return sf::String::fromUtf8(x.begin(),x.end()).toWideString();}
    static std::string safeStem(const std::string& x){std::string out;for(unsigned char ch:x){if((ch>='0'&&ch<='9')||(ch>='A'&&ch<='Z')||(ch>='a'&&ch<='z')||ch=='_'||ch=='-')out+=char(ch);else if(ch==' ')out+='_';}if(out.empty())out="match";return out.substr(0,35);}
    void typeName(sf::Uint32 ch){
        if(ch<32||ch==127)return; if(entry[editField].size()<26&&ch<=0xFFFF)entry[editField].push_back(wchar_t(ch));
    }
    static std::wstring trimmed(const std::wstring& value){
        auto first=value.find_first_not_of(L" \t\r\n");
        if(first==std::wstring::npos)return L"";
        auto last=value.find_last_not_of(L" \t\r\n");
        return value.substr(first,last-first+1);
    }
    static std::wstring normalizedName(const std::wstring& name){
        std::wstring lower=trimmed(name);
        for(auto& ch:lower)ch=std::towlower(ch);
        return lower;
    }
    bool nameAlreadySaved(const std::wstring& name) const {
        const auto needle=normalizedName(name);
        if(needle.empty())return false;
        for(const auto& path:availableManualSaves()){
            Game preview;
            if(preview.load(path)&&normalizedName(wide(preview.saveLabel))==needle)
                return true;
        }
        return false;
    }
    std::wstring nextGameName() const {
        // Find the first unused number, including old unaccented names (Van co 1).
        for(int n=1;n<10000;++n){
            const auto suffix=std::to_wstring(n);
            if(!nameAlreadySaved(L"Ván cờ "+suffix) &&
               !nameAlreadySaved(L"Van co "+suffix))return L"Ván cờ "+suffix;
        }
        return L"Ván cờ mới";
    }
    void openSetup(){
        editField=0;setupError.clear();
        entry[0]=L""; // New matches request a name rather than recycling an old placeholder.
        entry[1]=menuMode==0?L"Máy":L"";
        entry[2]=nextGameName();
        screen=Setup;
    }
    void confirmSetup(){
        setupError.clear();
        if(trimmed(entry[0]).empty()){
            setupError=tr(L"HÃY NHẬP TÊN NGƯỜI CHƠI ĐỎ",L"ENTER THE RED PLAYER NAME");
            editField=0;return;
        }
        if(menuMode==1 && trimmed(entry[1]).empty()){
            setupError=tr(L"HÃY NHẬP TÊN NGƯỜI CHƠI ĐEN",L"ENTER THE BLACK PLAYER NAME");
            editField=1;return;
        }
        auto label=trimmed(entry[2]);
        if(label.empty()){
            setupError=tr(L"VUI LÒNG NHẬP TÊN VÁN",L"ENTER A GAME NAME");
            editField=2;return;
        }
        if(nameAlreadySaved(label)){
            setupError=tr(L"TÊN VÁN ĐÃ TỒN TẠI - HÃY ĐỔI TÊN",L"GAME NAME EXISTS - CHOOSE ANOTHER");
            editField=2;return;
        }
        Game::Mode m=menuMode==0?Game::HumanVsAI:Game::HumanVsHuman;
        newMatch(m);
        game.redName=utf8(trimmed(entry[0]).empty()?L"Người chơi 1":trimmed(entry[0]));
        game.blackName=m==Game::HumanVsAI?"Bot":utf8(trimmed(entry[1]).empty()?L"Người chơi 2":trimmed(entry[1]));
        game.saveLabel=utf8(label);
    }
    void resetSelection(){selR=-1;selC=-1;options.clear();}
    void newMatch(Game::Mode m){botDelayClock.restart();game.start(m,120);namedSlot.clear();resetSelection();savedEnd=false;saveNotice.clear();cursorR=9;cursorC=4;screen=Match;std::cout<<"New match: 120s per turn.\n";}
    void endMatch(){if(game.state==Game::Playing||savedEnd)return;savedEnd=true;removeFinishedCheckpoint();resultElapsed=0.f;std::cout<<"Ket thuc: "<<Game::stateName(game.state)<<"\n";}
    void make(const Move& m){Piece captured=game.board.get(m.tr,m.tc);if(game.makeMove(m)){botDelayClock.restart();if(effectsEnabled){sound.stop();sound.setBuffer(captured.side==Piece::None?clickBuffer:captureBuffer);sound.setVolume(75.f);sound.play();}resetSelection();cursorR=m.tr;cursorC=m.tc;if(!game.log.empty())std::cout<<game.log.back()<<"\n";endMatch();}}
    void select(int r,int c){if(game.state!=Game::Playing)return;if(game.mode==Game::HumanVsAI&&game.turn!=Piece::Red)return;
        if(selR>=0){for(const Move& m:options)if(m.tr==r&&m.tc==c){make(m);return;}}
        if(game.board.get(r,c).side==game.turn){selR=r;selC=c;options=game.board.legalFrom(r,c);}else resetSelection();}
    void key(sf::Keyboard::Key k){
        if(k==sf::Keyboard::Escape){if(screen==Result&&resultElapsed<3.5f)return;screen=Menu;resetSelection();return;}
        if(screen==Setup){
            if(k==sf::Keyboard::Tab){editField=(editField+1)%3; if(menuMode==0&&editField==1)editField=2;}
            else if(k==sf::Keyboard::BackSpace){if(!entry[editField].empty())entry[editField].pop_back();}
            else if(k==sf::Keyboard::Return){confirmSetup();}
        }else if(screen==Match){
            if(k==sf::Keyboard::W||k==sf::Keyboard::Up)cursorR=std::max(0,cursorR-1);
            else if(k==sf::Keyboard::S||k==sf::Keyboard::Down)cursorR=std::min(9,cursorR+1);
            else if(k==sf::Keyboard::A||k==sf::Keyboard::Left)cursorC=std::max(0,cursorC-1);
            else if(k==sf::Keyboard::D||k==sf::Keyboard::Right)cursorC=std::min(8,cursorC+1);
            else if(k==sf::Keyboard::Return||k==sf::Keyboard::Space)select(cursorR,cursorC);
            else if(k==sf::Keyboard::F5)saveCurrent();
            else if(k==sf::Keyboard::H)screen=Help;
        }else if(screen==Menu){
            if(k==sf::Keyboard::Num1||k==sf::Keyboard::Numpad1)menuMode=0;
            if(k==sf::Keyboard::Num2||k==sf::Keyboard::Numpad2)menuMode=1;
            if(k==sf::Keyboard::Return)openSetup();
            if(k==sf::Keyboard::L)openHistory();
            if(k==sf::Keyboard::E){english=!english;saveSettings();}
            if(k==sf::Keyboard::O)screen=Settings;
        }
    }
    void loadLast(){
        auto saves=availableManualSaves();
        if(saves.empty()||!game.load(saves.front())){openSetup();return;}
        screen=Match;resultElapsed=0.f;resetSelection();savedEnd=false;
        namedSlot=std::filesystem::path(saves.front()).filename().string();
    }
    // The startup Replay button resumes the most recent MANUAL checkpoint only.
    void replayFromStartup(){loadLast();}
    void openHistory(){
        // Ignore legacy autosaves, README, settings and finished games.
        historyFiles=availableManualSaves();
        historyPage=0;selectedSave=0;screen=History;
    }
    void click(int x,int y){
        if(screen==Setup){
            if(hit(x,y,363,294,520,65))editField=0;
            else if(menuMode==1&&hit(x,y,363,411,520,65))editField=1;
            else if(hit(x,y,363,528,520,65))editField=2;
            else if(hit(x,y,390,687,210,60))confirmSetup();
            else if(hit(x,y,650,687,210,60))screen=Menu;
            return;
        }
        if(screen==Startup){
            if(hit(x,y,378,702,225,66)){replayFromStartup();return;}
            if(hit(x,y,647,702,225,66)){screen=Menu;return;}
            return;
        }
        if(screen==Menu){
            if(hit(x,y,1080,36,130,48)){window.close();return;}
            if(hit(x,y,980,36,90,48)){english=!english;saveSettings();return;}
            if(hit(x,y,423,310,195,66)){menuMode=0;saveSettings();return;}
            if(hit(x,y,633,310,195,66)){menuMode=1;saveSettings();return;}
            if(menuMode==0)for(int i=0;i<3;++i)if(hit(x,y,405+i*143,450,135,58)){difficulty=i;saveSettings();return;}
            if(hit(x,y,412,567,410,68)){openSetup();return;}
            if(hit(x,y,412,653,410,65)){openHistory();return;}
            if(hit(x,y,412,736,196,62)){screen=Help;return;}
            if(hit(x,y,626,736,196,62)){screen=Settings;return;}
        }else if(screen==Settings){
            if(hit(x,y,685,290,210,62)){effectsEnabled=!effectsEnabled;updateAudio();saveSettings();if(effectsEnabled){sound.stop();sound.setBuffer(clickBuffer);sound.play();}}
            else if(hit(x,y,685,390,210,62)){musicEnabled=!musicEnabled;updateAudio();saveSettings();}
            else if(hit(x,y,685,490,210,62)){english=!english;saveSettings();}
            else if(hit(x,y,512,747,230,66)){screen=Menu;}
        }else if(screen==Help){if(hit(x,y,489,825,260,58))screen=Menu;}
        else if(screen==History){
            if(hit(x,y,697,758,220,66)){screen=Menu;return;}
            if(hit(x,y,447,758,220,66)){if(selectedSave>=0&&selectedSave<int(historyFiles.size())){
                if(game.load(historyFiles[selectedSave])&&game.state==Game::Playing){ // Manually saved active game only
                    // Keep the saved clocks and active turn when resuming.
                    screen=Match;resultElapsed=0.f;resetSelection();savedEnd=false;namedSlot=std::filesystem::path(historyFiles[selectedSave]).filename().string();
                }
            }return;}
            if(hit(x,y,492,688,58,48)){historyPage=std::max(0,historyPage-1);return;}
            if(hit(x,y,705,688,58,48)){if((historyPage+1)*6<int(historyFiles.size()))++historyPage;return;}
            for(int i=0;i<6;++i){int idx=historyPage*6+i;
                if(idx<int(historyFiles.size())&&hit(x,y,310,225+i*71,640,58)){selectedSave=idx;return;}}
        }else if(screen==Result){
            if(resultElapsed<3.5f)return;
            if(hit(x,y,395,718,210,62)){auto red=game.redName,black=game.blackName,label=game.saveLabel;newMatch(game.mode);game.redName=red;game.blackName=black;game.saveLabel=label;return;}
            if(hit(x,y,645,718,210,62)){screen=Menu;resetSelection();return;}
        }else if(screen==Match){
            if(game.state!=Game::Playing){
                if(hit(x,y,145,455,220,58)){auto red=game.redName,black=game.blackName,label=game.saveLabel;newMatch(game.mode);game.redName=red;game.blackName=black;game.saveLabel=label;return;}
                if(hit(x,y,395,455,220,58)){screen=Menu;resetSelection();return;}
                return;
            }
            if(hit(x,y,870,705,330,55)){saveCurrent();return;}
            if(hit(x,y,870,772,330,55)){screen=Menu;resetSelection();return;}
            int c=int(std::round((x-bx-ox)/step)),r=int(std::round((y-by-oy)/step));
            if(game.board.inBounds(r,c)){float px=bx+ox+c*step,py=by+oy+r*step;
                if(std::abs(x-px)<=36&&std::abs(y-py)<=36){cursorR=r;cursorC=c;select(r,c);}}
        }
    }
    std::wstring timeString(float sec){int s=int(std::ceil(std::max(0.f,sec)));std::wostringstream o;if(s/60<10)o<<L"0";o<<(s/60)<<L":";if(s%60<10)o<<L"0";o<<s%60;return o.str();}
    std::wstring tr(const wchar_t* vi,const wchar_t* en)const{return english?std::wstring(en):std::wstring(vi);}
    std::wstring displayMove(const Move& move,const Piece& piece)const{
        const wchar_t* names[]={L"?",L"Tg",L"S",L"T",L"M",L"X",L"P",L"B"};
        const int from=piece.side==Piece::Red?9-move.fc:move.fc+1;
        const int to=piece.side==Piece::Red?9-move.tc:move.tc+1;
        const bool horizontal=move.fr==move.tr;
        const bool forward=piece.side==Piece::Red?move.tr<move.fr:move.tr>move.fr;
        const wchar_t direction=horizontal?L'-':(forward?L'.':L'/');
        const bool diagonal=piece.type==Piece::Horse||piece.type==Piece::Elephant||piece.type==Piece::Advisor;
        const int value=(diagonal||horizontal)?to:std::abs(move.tr-move.fr);
        return std::wstring(names[piece.type])+std::to_wstring(from)+direction+std::to_wstring(value);
    }
    void drawBackdrop(){
        sf::Sprite bg(landscape);
        if(landscape.getSize().x>0){bg.setScale(1250.f/landscape.getSize().x,940.f/landscape.getSize().y);window.draw(bg);}
        else rect(0,0,1250,940,sf::Color(25,140,210));
    }
    void panel(float x,float y,float w,float h){
        rect(x+7,y+7,w,h,sf::Color(79,48,24,110));
        rect(x,y,w,h,sf::Color(234,210,172,245));
        rect(x,y,w,5,sf::Color(122,75,38));rect(x,y+h-5,w,5,sf::Color(122,75,38));
        rect(x,y,5,h,sf::Color(122,75,38));rect(x+w-5,y,5,h,sf::Color(122,75,38));
    }
    void updateAudio(){
        // sf::Sound (SFX) and sf::Music (BGM) are independent sources.
        // Muting music must NEVER mute the effects channel.
        bgMusic.setVolume(musicEnabled?20.f:0.f);
        sound.setVolume(effectsEnabled?75.f:0.f);
        if(musicEnabled && bgMusic.getStatus()!=sf::Music::Playing)bgMusic.play();
        if(!musicEnabled && bgMusic.getStatus()==sf::Music::Playing)bgMusic.pause();
    }
    void saveSettings(){std::filesystem::create_directories(saveDirectory());std::ofstream f(saveDirectory() / "settings.txt");if(f)f<<english<<' '<<musicEnabled<<' '<<effectsEnabled<<' '<<difficulty<<' '<<menuMode<<'\n';}
    void loadSettings(){std::ifstream f(saveDirectory() / "settings.txt");if(f)f>>english>>musicEnabled>>effectsEnabled>>difficulty>>menuMode;
        difficulty=std::clamp(difficulty,0,2);menuMode=std::clamp(menuMode,0,1);}
    void drawSettings(){
        drawBackdrop();panel(328,165,605,665);
        text(tr(L"CÀI ĐẶT",L"SETTINGS"),630,199,34,sf::Color(110,30,20),true);
        text(tr(L"Âm thanh",L"Sound effects"),365,306,21);
        button(effectsEnabled?tr(L"BẬT",L"ON"):tr(L"TẮT",L"OFF"),685,290,210,62,effectsEnabled);
        text(tr(L"Nhạc nền",L"Background music"),365,406,21);
        button(musicEnabled?tr(L"BẬT",L"ON"):tr(L"TẮT",L"OFF"),685,390,210,62,musicEnabled);
        text(tr(L"Ngôn ngữ",L"Language"),365,506,21);
        button(english?L"ENGLISH":L"TIẾNG VIỆT",685,490,210,62,true);
        
        button(tr(L"QUAY LẠI",L"BACK"),512,747,230,66);
    }
    void drawStartup(){
        // Show the supplied red autumn pixel-art without a dark overlay or a large title.
        drawBackdrop();
        coloredButton(tr(L"SẴN SÀNG",L"READY"),378,702,225,66,sf::Color(67,143,91));
        woodButton(tr(L"MENU CHÍNH",L"MAIN MENU"),647,702,225,66);
    }
    void drawMenu(){
        drawBackdrop();
        button(english?L"VI":L"EN",980,36,90,48);button(tr(L"THOÁT",L"EXIT"),1080,36,130,48);
        panel(326,166,600,672);
        text(tr(L"CHẾ ĐỘ CHƠI",L"GAME MODE"),625,210,23,sf::Color(110,30,20),true);
        button(tr(L"ĐẤU MÁY",L"VS BOT"),423,310,195,66,menuMode==0);
        button(tr(L"2 NGƯỜI",L"2 PLAYERS"),633,310,195,66,menuMode==1);
        text(tr(L"ĐỘ KHÓ",L"DIFFICULTY"),625,412,22,sf::Color(110,30,20),true);
        // Luon hien thi nut do kho mau xanh, ke ca khi choi 2 nguoi.
        // Xu ly click chi cho phep thay doi do kho khi dau may.
        button(tr(L"DỄ",L"EASY"),405,450,135,58,difficulty==0,menuMode==0);
        button(tr(L"VỪA",L"MEDIUM"),548,450,135,58,difficulty==1,menuMode==0);
        button(tr(L"KHÓ",L"HARD"),691,450,135,58,difficulty==2,menuMode==0);
        text(tr(L"Chỉ áp dụng khi đấu máy",L"Only for VS BOT"),625,516,16,sf::Color(36,20,9),true);
        button(tr(L"VÁN MỚI",L"NEW GAME"),412,567,410,68);
        button(tr(L"TẢI VÁN CŨ",L"LOAD GAME"),412,653,410,65);
        button(tr(L"HƯỚNG DẪN",L"HOW TO PLAY"),412,736,196,62);
        button(tr(L"CÀI ĐẶT",L"SETTINGS"),626,736,196,62);
    }
    void drawSetup(){
        drawBackdrop();panel(310,130,630,690);
        text(tr(L"THÔNG TIN VÁN ĐẤU",L"MATCH DETAILS"),625,165,31,sf::Color(110,30,20),true);
        const std::wstring captions[3]={tr(L"TÊN QUÂN ĐỎ",L"RED PLAYER"),tr(L"TÊN QUÂN ĐEN",L"BLACK PLAYER"),tr(L"TÊN VÁN LƯU",L"SAVE NAME")};
        for(int i=0;i<3;++i){
            const float y=264+i*117;bool locked=i==1&&menuMode==0;
            text(captions[i],364,y,19,sf::Color(36,20,9));
            rect(363,y+30,520,65,sf::Color(246,230,203));
            rect(363,y+30,520,2,sf::Color(122,75,38));
            if(editField==i&&!locked){rect(363,y+30,520,3,sf::Color(122,32,24));}
            std::wstring val=entry[i];if(locked)val=tr(L"MÁY",L"BOT");
            if(val.empty()){
                text(tr(L"Nhập tên...",L"Enter name..."),382,y+46,20,sf::Color(110,90,70));
            }else{
                text(val,382,y+46,22,sf::Color(36,20,9));
            }
        }
        if(!setupError.empty())
            text(setupError,625,638,17,sf::Color(166,44,33),true);
        else
            text(tr(L"Nhấn vào ô để nhập • Enter để bắt đầu",L"Click to type • Enter to start"),625,638,17,sf::Color(36,20,9),true);
        button(tr(L"BẮT ĐẦU",L"START"),390,687,210,60);
        button(tr(L"QUAY LẠI",L"BACK"),650,687,210,60);
    }
    void drawHelp(){
        drawBackdrop();panel(153,96,944,788);
        text(tr(L"HƯỚNG DẪN",L"HOW TO PLAY"),625,130,34,sf::Color(110,30,20),true);
        const std::vector<std::wstring> vi={L"CHỌN QUÂN",L"Click vào một quân hoặc WASD + Enter để chọn.",L"Các vị trí sáng là nước đi hợp lệ. Click ô đích để đi.",L"LUẬT CƠ BẢN",L"Tướng đi một ô trong cung. Sĩ đi chéo một ô trong cung.",L"Tượng đi chéo hai ô, không qua sông và không bị chặn mắt.",L"Mã đi chữ L (có thể bị cản chân). Xe đi ngang hoặc dọc.",L"Pháo ăn quân khi có đúng một quân làm ngòi.",L"Tốt tiến một ô; qua sông có thể đi ngang, không lùi.",L"LƯU VÀ THAO TÁC",L"F5: Lưu nhanh. ESC: Menu. H: Hướng dẫn."};
        const std::vector<std::wstring> en={L"SELECT A PIECE",L"Click a piece or use WASD + Enter to select.",L"Highlighted intersections are legal moves. Click to move.",L"BASIC RULES",L"General: one step inside palace. Advisor: diagonal in palace.",L"Elephant: two diagonally, cannot cross river or jump eyes.",L"Horse: L-shape, can be blocked. Rook: straight lines.",L"Cannon captures by jumping exactly one screen piece.",L"Soldier advances; can go sideways after the river.",L"SAVES AND CONTROLS",L"F5: quick save. ESC: menu. H: help."};
        const auto& lines=english?en:vi;
        for(size_t i=0;i<lines.size();++i){bool head=i==0||i==3||i==9;text(lines[i],200,218+49*i,head?22:17,head?sf::Color(110,30,20):sf::Color(36,20,9));}
        button(tr(L"QUAY LẠI",L"BACK"),489,825,260,58);
    }
    void drawHistory(){
        drawBackdrop();panel(245,108,770,740);
        text(tr(L"TẢI VÁN CŨ",L"LOAD GAME"),625,138,32,sf::Color(110,30,20),true);
        if(historyFiles.empty())text(tr(L"Chưa có ván đấu nào được lưu",L"No saved games yet"),625,350,20,sf::Color(36,20,9),true);
        for(int i=0;i<6;++i){int idx=historyPage*6+i;if(idx>=int(historyFiles.size()))break;
            std::string raw=std::filesystem::path(historyFiles[idx]).stem().string();
            std::wstring label(raw.begin(),raw.end());
            if(raw=="last_game")label=tr(L"Ván đang chơi (tự lưu)",L"Latest autosave");
            else {Game preview;if(preview.load(historyFiles[idx]))label=wide(preview.saveLabel)+L"  ("+wide(preview.redName)+L" - "+wide(preview.blackName)+L")";}
            button(label,310,225+i*71,640,58,idx==selectedSave);
        }
        button(L"<",492,688,58,48);button(L">",705,688,58,48);
        text(std::to_wstring(historyPage+1),628,700,19,sf::Color(36,20,9),true);
        button(tr(L"TẢI ĐÃ CHỌN",L"LOAD SELECTED"),447,758,220,66);
        button(tr(L"QUAY LẠI",L"BACK"),697,758,220,66);
    }
    void drawResult(){
        // Show the original red autumn pixel-art unobstructed first.
        drawBackdrop();
        if(resultElapsed<1.8f)return;
        // Semi-transparent title does not replace the user's artwork.
        bool redWon=game.state==Game::RedWins||game.state==Game::BlackTimeout;
        bool blackWon=game.state==Game::BlackWins||game.state==Game::RedTimeout;
        bool playerWon=(game.mode==Game::HumanVsAI && redWon);
        bool playerLost=(game.mode==Game::HumanVsAI && blackWon);
        std::wstring result;
        if(playerWon)result=tr(L"BẠN ĐÃ THẮNG!",L"YOU WIN!");
        else if(playerLost)result=tr(L"BẠN ĐÃ THUA!",L"YOU LOSE!");
        else result=redWon?tr(L"QUÂN ĐỎ THẮNG!",L"RED WINS!"):tr(L"QUÂN ĐEN THẮNG!",L"BLACK WINS!");
        rect(215,240,820,165,sf::Color(15,25,36,185));
        if(game.mode==Game::HumanVsHuman)result=wide(redWon?game.redName:game.blackName)+tr(L" ĐÃ THẮNG!",L" WINS!");
        centeredText(result,625,322,44,redWon?sf::Color(255,211,109):sf::Color(245,192,187));
        if(game.state==Game::RedTimeout||game.state==Game::BlackTimeout)
            text(tr(L"HẾT 30 GIÂY SUY NGHĨ",L"30-SECOND TURN TIME EXPIRED"),625,357,20,sf::Color::White,true);
        if(resultElapsed<3.5f)return;
        button(tr(L"VÁN TIẾP",L"NEXT GAME"),395,718,210,62);
        button(tr(L"MENU CHÍNH",L"MAIN MENU"),645,718,210,62);
    }
    void drawCircle(float x,float y,float radius,sf::Color fill,sf::Color outline,float border){sf::CircleShape s(radius);s.setPosition(x-radius,y-radius);s.setFillColor(fill);s.setOutlineColor(outline);s.setOutlineThickness(border);window.draw(s);}
    void drawMatch(){
        sf::Sprite bg(boardImage);bg.setPosition(bx,by);window.draw(bg);
        if(game.hasLast){for(auto rc: {std::pair<int,int>(game.lastMove.fr,game.lastMove.fc),std::pair<int,int>(game.lastMove.tr,game.lastMove.tc)}){drawCircle(bx+ox+rc.second*step,by+oy+rc.first*step,32,sf::Color(240,192,57,55),sf::Color(243,169,55,160),2);}}
        if(selR>=0){drawCircle(bx+ox+selC*step,by+oy+selR*step,35,sf::Color(100,217,180,45),sf::Color(86,232,180),3);for(const Move& m:options){auto p=game.board.get(m.tr,m.tc);drawCircle(bx+ox+m.tc*step,by+oy+m.tr*step,p.side==Piece::None?12:31,sf::Color(50,198,136,165),sf::Color(28,147,108,210),2);}}
        drawCircle(bx+ox+cursorC*step,by+oy+cursorR*step,38,sf::Color::Transparent,sf::Color(46,159,232,240),3);
        for(int r=0;r<10;++r)for(int c=0;c<9;++c){Piece p=game.board.get(r,c);if(p.side==Piece::None)continue;sf::Sprite sprite(pieces[p.side==Piece::Red?0:1][p.type]);sprite.setOrigin(sprite.getTexture()->getSize().x/2.f,sprite.getTexture()->getSize().y/2.f);sprite.setPosition(bx+ox+c*step,by+oy+r*step);window.draw(sprite);}
        rect(858,18,370,906,sf::Color(122,75,38));rect(862,22,362,898,sf::Color(234,210,172));
        centeredText(tr(L"CỜ TƯỚNG",L"XIANGQI"),1043,60,42,sf::Color(36,20,9));
        centeredText(wide(game.blackName),1043,111,18,sf::Color(36,20,9));
        centeredText(wide(game.redName),1043,143,18,sf::Color(36,20,9));
        if(game.state==Game::Playing)centeredText(timeString(game.turn==Piece::Red?game.redTime:game.blackTime),1043,180,38,sf::Color(36,20,9));
        std::wstring status;if(game.state==Game::Playing)status=game.turn==Piece::Red?tr(L"LƯỢT ĐỎ",L"RED TURN"):tr(L"LƯỢT ĐEN",L"BLACK TURN");else if(game.state==Game::RedWins)status=tr(L"ĐỎ THẮNG!",L"RED WINS!");else if(game.state==Game::BlackWins)status=tr(L"ĐEN THẮNG!",L"BLACK WINS!");else if(game.state==Game::Draw)status=tr(L"VÁN HÒA",L"DRAW");else status=tr(L"HẾT GIỜ!",L"TIME UP!");
        centeredText(status,1043,218,24,sf::Color(36,20,9));
        centeredText(tr(L"LỊCH SỬ NƯỚC ĐI",L"MOVE HISTORY"),1043,288,24,sf::Color(36,20,9));
        rect(875,309,335,2,sf::Color(122,75,38));
        rect(874,314,337,238,sf::Color(122,75,38));rect(878,318,329,230,sf::Color(246,230,203));
        centeredText(tr(L"LƯỢT",L"NO."),914,331,15,sf::Color(36,20,9));centeredText(tr(L"ĐỎ",L"RED"),1017,331,15,sf::Color(36,20,9));centeredText(tr(L"ĐEN",L"BLACK"),1142,331,15,sf::Color(36,20,9));
        std::vector<std::wstring> moveNotation;
        moveNotation.reserve(game.moves.size());
        Board notationBoard;notationBoard.reset();
        for(const Move& move:game.moves){
            const Piece moving=notationBoard.get(move.fr,move.fc);
            if(moving.side==Piece::None)break;
            moveNotation.push_back(displayMove(move,moving));
            notationBoard.apply(move);
        }
        int rows=int((moveNotation.size()+1)/2),first=std::max(0,rows-7);
        for(int r=first;r<rows;++r){
            float y=357+(r-first)*27;
            rect(878,y-2,329,25,r%2==0?sf::Color(246,230,203):sf::Color(223,194,152));
            centeredText(std::to_wstring(r+1),914,y+10,16,sf::Color(36,20,9));
            centeredText(moveNotation[2*r],1017,y+10,17,sf::Color(36,20,9));
            if(2*r+1<int(moveNotation.size()))centeredText(moveNotation[2*r+1],1142,y+10,17,sf::Color(36,20,9));
        }
        woodButton(tr(L"F5  LƯU VÁN",L"F5  SAVE"),870,705,330,55);woodButton(tr(L"ESC  VỀ MENU",L"ESC  MENU"),870,772,330,55);
        if(!saveNotice.empty()&&noticeClock.getElapsedTime().asSeconds()<3.5f){
            rect(873,847,342,49,sf::Color(43,29,20,240));
            text(saveNotice,1043,859,16,noticeColor,true);
        }
        if(game.state!=Game::Playing){
            if(game.state==Game::Draw){
                rect(80,330,610,205,sf::Color(26,18,13,225));
                rect(80,330,610,3,sf::Color(212,175,55));rect(80,532,610,3,sf::Color(212,175,55));
                rect(80,330,3,205,sf::Color(212,175,55));rect(687,330,3,205,sf::Color(212,175,55));
                centeredText(tr(L"VÁN CỜ HÒA",L"DRAW"),385,390,24,sf::Color(243,229,171));
                woodButton(tr(L"VÁN TIẾP",L"NEXT GAME"),145,455,220,58);
                woodButton(tr(L"VỀ MENU",L"MAIN MENU"),395,455,220,58);
                return;
            }
            const bool redWon=game.state==Game::RedWins||game.state==Game::BlackTimeout;
            rect(80,330,610,205,sf::Color(26,18,13,225));
            rect(80,330,610,3,sf::Color(212,175,55));rect(80,532,610,3,sf::Color(212,175,55));
            rect(80,330,3,205,sf::Color(212,175,55));rect(687,330,3,205,sf::Color(212,175,55));
            const std::wstring result=redWon?tr(L"PHE ĐỎ THẮNG  -  PHE ĐEN THUA",L"RED WINS  -  BLACK LOSES"):tr(L"PHE ĐEN THẮNG  -  PHE ĐỎ THUA",L"BLACK WINS  -  RED LOSES");
            centeredText(result,385,390,22,sf::Color(243,229,171));
            woodButton(tr(L"VÁN TIẾP",L"NEXT GAME"),145,455,220,58);
            woodButton(tr(L"VỀ MENU",L"MAIN MENU"),395,455,220,58);
        }
    }
};
int runApplication(){try{Application app;app.run();}catch(const std::exception& ex){std::cerr<<"Loi: "<<ex.what()<<'\n';return 1;}return 0;}
