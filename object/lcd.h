#ifndef __LCD_H__
#define __LCD_H__

/**
 * @brief LCD_INIT 初始化屏幕
 * @return int 成功返回0，失败返回-1
*/
int LCD_INIT();

int LCD_UNINIT();

/**
 * @brief DrawPixel 画点函数
 * @param h 像素点所在行
 * @param w 像素点所在列
 * @param color 像素点需要显示的颜色
*/
void DrawPixel(int h,int w,int color);

//画棋盘线
void Drawboard(int color);

//画背景
void DrawBackgournd(int color);

/* 打印图片
    @posx:图片x轴偏移量
    @posy:图片y轴偏移量
    @pic_path:图片名
*/
int DisplayBMPPicture(int posx,int posy,const char *pic_path);

//显示棋盘
void Darw_board(int first,char board[ROW][COL]);

//获取手指点击屏幕的坐标,超出范围返回1,未超出范围返回-1
void Get_XY(int *x,int *y);

//获取手指点击屏幕的坐标之后进行开始游戏的操作
void Start_game(char board[ROW][COL],char displayBoard[ROW][COL],int Y,int X);

//初始化界面
void Initial();

//重新开始游戏与退出游戏程序
int Restart();

#endif