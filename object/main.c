#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/fb.h>
#include <stdlib.h>
#include <linux/input.h>
#include <time.h>
#include "mines.h"
#include "lcd.h"

int main()
{
    int first = 1;
    char displayBoard[ROW][COL];//展示棋盘
    char board[ROW][COL];//内部数据棋盘

    //初始化屏幕
    LCD_INIT();

    //初始化游戏界面,并限制进入游戏区域
    Initial();

    //第一次无条件开始游戏，first跟Restart不能替换位置，否则需要点两下
    while (first || Restart())
    {
        Creat_board(board);//初始化内部棋盘
        Creat_board(displayBoard);//初始化展示棋盘

        //设置背景板
        DrawBackgournd(0xffffff);
        
        int X = -1;//初始化坐标
        int Y = -1;

        //显示棋盘,最开始的时候无法结束游戏
        Darw_board(0,board);

        //感应触摸屏幕将XY值修改成点击的坐标
        Get_XY(&Y,&X);
        
        //获取到坐标之后开始进行游戏
        Start_game(board,displayBoard,Y,X);

        //将frist置零，之后的循环条件都是判断Restart
        first = 0;
    }
    //游戏结束
    DisplayBMPPicture(0,0,"gameover.bmp");

    //关闭屏幕
    LCD_UNINIT();
    return 0;
}
