#ifndef __MINES_H__
#define __MINES_H__

#define ROW 11
#define COL 17

//初始化棋盘
void Creat_board(char board[ROW][COL]);

//获取该格子周围雷的数量
int countMines(char board[ROW][COL], int row, int col);

//打印棋盘
void print_board(char board[ROW][COL]);

//随机落雷(在第一次选点的3×3范围外)
void DownMines(char board[ROW][COL],int row, int col);

//将数据计算并加入棋盘
void InsertData(char board[ROW][COL],int row,int col);

//获取棋盘中字符a的数量
int Find_sign(char board[ROW][COL],char a);

/* 判断游戏是否成功 
    @return:1表示成功
            0表示还未成功
*/
int Judge_func(char board[ROW][COL]);

//将内部棋盘可展示部分赋值给展示棋盘，并递归找空地
void revealCell(char board[ROW][COL], char displayBoard[ROW][COL], int row, int col);

#endif