#include<windows.h>
#include<stdio.h>
#include<time.h>
#include<conio.h>       //提供 顏色函數
#include<iostream>      //提供 顏色函數
#include<thread>        //製作多執行緒

using namespace std;

void SetColor(int f=7,int b=8){
    unsigned short ForeColor=b+16*f;
    HANDLE hCon = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hCon,ForeColor);
} //設定輸出顏色的函數(由兩個參數控制 前景色、背景色)

int radom(){
    srand(time(NULL));
    return (rand()%7)+9;
}   // 回傳9~15的亂數(為了配合色碼方塊的代號是9~15)

class print{
    COORD pos;
    bool busy = false;
public:
    void P(int *X,int *Y,int color,string s,int K){   //在螢幕上輸出方塊
        while(busy) Sleep(00001);
        busy = true;
        HANDLE hin = GetStdHandle(STD_OUTPUT_HANDLE);
        for(int i=0;i<4;i++){
            pos.X = X[i]*2+K, pos.Y = Y[i];
            SetConsoleCursorPosition(hin, pos);
            SetColor(color);
            cout << s;
        }
        SetColor();
        hin = GetStdHandle(STD_OUTPUT_HANDLE);
        pos.X = 20, pos.Y = 24;
        SetConsoleCursorPosition(hin, pos);
        busy = false;
    }
    void P(int a,int b){                //在螢幕上輸出成績
        while(busy) Sleep(00001);
        busy = true;
        HANDLE hin = GetStdHandle(STD_OUTPUT_HANDLE);
        SetColor(7,9);
        pos.X = 5, pos.Y = 20;
        SetConsoleCursorPosition(hin, pos);
        printf("score:%d\n     best score:%d",a,b);
        SetColor();
        hin = GetStdHandle(STD_OUTPUT_HANDLE);
        pos.X = 20, pos.Y = 24;
        SetConsoleCursorPosition(hin, pos);
        busy = false;
    }
}print;

struct BG{
    bool exist=0;
    int color=0;
};

class Game{             //遊戲的參數
protected:
    int bl_num=0;
    int btm=0;
    bool bl_temp=false;
    bool bl_change=false;
    bool working = false;
    bool G_Start = false;
    bool gameover = false;
    int score=0;
    int bestscore;
    BG bg[20][10];
};

class blocks : protected Game{
private:
    int x[4],y[4];                  //紀錄方塊內每一個格子的位置
    int blcolor=13;                 // 記錄顏色
    int id,ptn=0;                   // id:方塊的種類(有七種方塊)  ptn:紀錄方塊的姿勢
    const int bl_ori_x[7][4] = {{0,1,1,1},{0,0,1,1},{0,0,0,0},{0,0,1,1},{1,1,1,0},{0,0,1,1},{0,1,1,1}};
    const int bl_ori_y[7][4] = {{3,3,4,5},{4,5,3,4},{3,4,5,6},{3,4,4,5},{3,4,5,4},{4,5,4,5},{5,3,4,5}};
    const int X[4] = {-1,0,1,0};
    const int Y[4] = {0,-1,0,1};
    const int bl_pose_x[7][4][4] ={  //方塊不同姿勢的相對坐標
            {{-2,-1,0,1},{0,-1,0,1},{1,0,-1,-2},{1,2,1,0}},
            {{-1,-2,1,0},{1,2,-1,0},{-1,-2,1,0},{1,2,-1,0}},
            {{1,0,-1,-2},{-1,0,1,2},{1,0,-1,-2},{-1,0,1,2}},
            {{0,-1,0,-1},{0,1,0,1},{0,-1,0,-1},{0,1,0,1}},
            {{-1,0,1,-1},{-1,0,1,1},{1,0,-1,1},{1,0,-1,-1}},
            {{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0}},
            {{0,-1,0,1},{2,-1,0,1},{-1,0,-1,-2},{-1,2,1,0}}
        };
    const int bl_pose_y[9][4][4] ={  //方塊不同姿勢的相對坐標
        {{-1,-2,-1,0},{1,0,-1,-2},{1,2,1,0},{-1,0,1,2}},
        {{-1,0,-1,0},{1,0,1,0},{-1,0,-1,0},{1,0,1,0}},
        {{-1,0,1,2},{1,0,-1,-2},{-1,0,1,2},{1,0,-1,-2}},
        {{-1,0,1,2},{1,0,-1,-2},{-1,0,1,2},{1,0,-1,-2}},
        {{-1,0,1,1},{1,0,-1,1},{1,0,-1,-1},{-1,0,1,-1}},
        {{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0}},
        {{1,-2,-1,0},{-1,0,-1,-2},{-1,2,1,0},{1,0,1,2}}
    };
public:
    bool gmo_check(){                                       //判斷遊戲結束
        if(bg[0][4].exist==true||bg[0][3].exist==true||bg[0][5].exist==true) return true;
    }
    bool fore_check(){                                      // 1右邊 0左邊
        short int d=2;
        for(int i=0;i<4;i++){
            if(bg[x[i]][y[i]].exist==true) return false;
            if(x[i]>=20) return false;
            if(y[i]<0) d=1;
            if(y[i]>=10) d=0;
        }
        if(d==1) op_bl(1);
        else if(d==0) op_bl(0);
        return true;
    }
    void set_bl(int a){             //設定 方塊(參數a決定方塊的形狀)
        id = a;
        ptn=0;
        for(int i=0;i<4;i++){
            x[i] = bl_ori_x[a-9][i];
            y[i] = bl_ori_y[a-9][i];
        }
        blcolor=a;
    }
    void op_bl(int a){              // 控制左右 (參數1代表向右 0向左)
        for(int i=0;i<4;i++){
            y[i]+= (a*2)-1;
        }
    }
    bool check_bl(int a){           //檢查方塊能否放在該位置
        a-=1;
        for(int i=0;i<4;i++){           // 上下左右
            if( x[i]+X[a] < 0 || y[i]+Y[a] < 0 ) return false;
            if( x[i]+X[a] >= 20 || y[i]+Y[a] >= 10) return false;
            if(bg[x[i]+X[a]][y[i]+Y[a]].exist) return false;
        }
        return true;
    }
    void down_bl(){                 //方塊落下
        for(int i=0;i<4;i++){
            x[i] +=1;
        }
    }
    void stop_bl(){                 //掉到底下了
        for(int i=0;i<4;i++){
            bg[x[i]][y[i]].exist=true;
            bg[x[i]][y[i]].color=blcolor;
        }
    }
    void show_bl(){             //在螢幕上輸出方塊
        print.P(y,x,blcolor,"  ",0);
    }
    void show_bl_t(){           //在螢幕上輸出使用者換的方塊
        print.P(y,x,blcolor," .",20);
    }
    void clr_bl(){              //刪掉該方塊上一秒的位置
        print.P(y,x,0," .",0);
    }
    void clr_bl_t(){            //刪掉使用者換的方塊
        print.P(y,x,7,"  ",20);
    }
    void turn(){                //旋轉方塊
        ptn = (ptn+1)%4;
        for(int i=0;i<4;i++){
            x[i] += bl_pose_x[id-9][ptn][i];
            y[i] += bl_pose_y[id-9][ptn][i];
        }
    }
    void turn_back(){       //逆轉判斷(用於判斷方塊)
        turn();turn();turn();
    }
};






