#include<windows.h>
#include<stdio.h>
#include<time.h>
#include<conio.h>           //提供 顏色函數
#include<iostream>          //提供 顏色函數
#include<thread>            //處理多執行緒的函式庫
#include"blocks.h"
using namespace std;

class op:private blocks{
blocks test;
bool dir_con = true;
public:
    void New_Game(){
        G_Start = true;
    }
    void record_write(){                  //讀寫檔案 (保存最高紀錄)
        FILE *fp;
        fp = fopen("score.txt","w");
        fprintf(fp,"%d",score);
        fclose(fp);
        return ;
    }
    void record_read(){                   //讀寫檔案 (保存最高紀錄)
        FILE *fp;
        fp = fopen("score.txt","r");
        fscanf(fp,"%d",&bestscore);
        fclose(fp);
        return;
    }
    void pscore(){                       //在底下顯示成績
        print.P(score,bestscore);
    }
    void draw(){                    //畫出遊戲的背景
        system("cls");
        for(int i=0;i<20;i++){
            for(int n=0;n<10;n++){
                SetColor(bg[i][n].color);
                printf(" .");
                SetColor();
            }
            printf("\n");
        }
        pscore();
    }
    void reset(){               //新的一局數據歸零
        for(int i=0;i<10;i++){
            for(int n=0;n<20;n++){
                bg[n][i].exist = false;
                bg[n][i].color=0;
            }
        }
    }
    void start(){               //遊戲本體
        score = 0;
        draw();
        clean(0);
        gameover = false;
        bl_num=radom();
        while(!gameover){
            set_bl(bl_num);           //隨機一個方塊
            dir_con=true;           //另一邊的開始接受對 方塊的指令
            bool stop = false;
            while(stop==false){
                working = true;        //遊戲顯示中 讓使用者的指另先暫緩
                show_bl();
                working = false;
                Sleep(500);         //每隔半秒 方塊下降
                while(working) Sleep(0001);
                if(bl_change){          //使用者想要換方塊
                    clr_bl();
                    working = false;
                    break;
                }
                else if(check_bl(3)) {  //判斷是否可以下降
                    clr_bl();
                    down_bl();
                }
                else  {                   //到底了 => 判斷是否消除
                    working = true;
                    stop_bl();          //停住
                    stop=true;          //不能再控制該方塊
                    goal();             //判斷消除幾行
                    working = false;
                }
            }
            if(bl_change) bl_change=false;
            else bl_num=radom();        // radom 一個新的方塊
            gameover=(gmo_check());     //判斷是否遊戲結束
            if(gameover) G_Start = false;
        }
    }
    void clean(int a){                  //刪掉第a行
        for(int i=0;i<10;i++){
            bg[a][i].color=0;
            bg[a][i].exist=false;
        }
        for(int i=a;i>0;i--){
            for(int n=0;n<10;n++){
            swap(bg[i][n].color,bg[i-1][n].color);
            swap(bg[i][n].exist,bg[i-1][n].exist);
            }
        }
        draw();
    }
    void goal(){                        //消除方塊、計算成績
        for(int i=0;i<20;i++){
            bool strike=true;
            for(int k=0;k<10;k++){
                if(!bg[i][k].exist){
                    strike=false ;
                    break;
                }
            }
            if(strike) {                //該行要被消除
                clean(i);
                score+=100;
                if(score>bestscore) {
                    record_write();
                    record_read();
                }
            }
        }
        pscore();
    }
    void G_Over(){          //遊戲結束的文字
        SetColor(7,12);
        cout << "\n         gameover!\n" ;
        reset();
        cout << "      please enter \"r\" to restart :" ;
    }
    friend void window_get();       //讓 處理輸入的函數可以讀到 遊戲的資料
}Tetris;

void window_get(){        //處理使用者的輸入 為遊戲的friend函數 故可以存取遊戲的方塊的資料(protected)
    while(Tetris.G_Start){
        while(Tetris.gameover==false){
            char c;
            if(Tetris.dir_con){
                while(Tetris.working) Sleep(0001);
                c=getch();
                Tetris.working=true;
                if(c==' '){                 //直接落下
                    while(Tetris.check_bl(3)) {
                        Tetris.dir_con=false;
                        Tetris.clr_bl();
                        Tetris.down_bl();
                        Tetris.show_bl();
                    }
                    Tetris.working=false;
                }
                else if(c=='c'){            //使用者要換方塊
                    if(!Tetris.bl_temp){
                        Tetris.btm=Tetris.bl_num;
                        Tetris.bl_num=radom();
                        Tetris.bl_temp=true;
                        Tetris.bl_change=true;
                        Tetris.working=false;
                        Tetris.test.set_bl(Tetris.btm);
                        Tetris.test.show_bl_t();
                    }
                    else {
                        swap(Tetris.btm,Tetris.bl_num);
                        Tetris.bl_change=true;
                        Tetris.working=false;
                        Tetris.test.clr_bl_t();
                        Tetris.test.set_bl(Tetris.btm);
                        Tetris.test.show_bl_t();
                    }
                }
                else if(c==-32){            //為方向鍵
                    c=getch();
                    switch(c){              //分四個case 去處理上下左右
                        case 72:
                            Tetris.clr_bl();
                            Tetris.turn();
                            if(!Tetris.fore_check()) Tetris.turn_back();
                            Tetris.show_bl();
                            Tetris.working=false;
                            break;
                        case 80:
                            if(Tetris.check_bl(3)) {
                                Tetris.clr_bl();
                                Tetris.down_bl();
                                Tetris.show_bl();
                            }
                            Tetris.working=false;
                            break;
                        case 75:
                            if(Tetris.check_bl(2)) {
                                Tetris.clr_bl();
                                Tetris.op_bl(0);
                                Tetris.show_bl();
                            }
                            Tetris.working=false;
                            break;
                        case 77:
                            if(Tetris.check_bl(4)) {
                                Tetris.clr_bl();
                                Tetris.op_bl(1);
                                Tetris.show_bl();
                            }
                            Tetris.working=false;
                            break;
                        default : break;
                    }
                }
                else Tetris.working= false;
            }
        }
    }
}

int main(){
    Tetris.record_read();
    char c;
    do{
        system("pause");
        Tetris.New_Game();
        thread wg(window_get);
        Tetris.start();
        Tetris.G_Over();
        wg.join();              //多執行緒結束

        cin >> c;
        if(c != 'r') break;
    } while (c == 'r'||c == 'R');
    return 0;
}
