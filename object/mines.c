#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ROW 11
#define COL 17
#define MINES 10

//初始化棋盘
void Creat_board(char board[ROW][COL])
{
    // 初始化二维数组空间
    for(int h = 0;h < ROW;h++)
    {  
        for(int w = 0;w < COL;w++)
        {
            board[h][w] = '#';//初始化二维矩阵
        }
    }
}

//打印棋盘
void print_board(char board[ROW][COL])
{
    for(int h = 0;h < ROW;h++)
    {  
        for(int w = 0; w < COL; w++)
        {
            /* if (board[h-1][w] != '#')
            {
                if (board[h-1][w] == 'X')
                {   //将旗子颜色设置为蓝色
                    printf("%c  ",board[h-1][w]);
                }
                else
                //为了美观将边界打印为红色
                printf("%c  ",board[h-1][w]);
            }
            else */
            printf("%c  ",board[h][w]);//打印元素
        }
        printf("\n\n\n");
    }
}

//随机落雷(在第一次选点的3×3范围外)
void DownMines(char board[ROW][COL],int row, int col)
{
    srand(time(0));//随机种子
    int count = 0;
    //先将第一次输入的点的周围变成雷
    for (int i = row - 1; i <= row + 1; i++)
    {
        for (int j = col - 1; j <= col + 1; j++)
        {
            board[i][j] = '*';//随机生成雷时就不会在这个点生成
        }
    }
    //随机落十个雷
    while (count < MINES)
    {
        int x = rand() % ROW;
        int y = rand() % COL;
        if (board[x][y] != '*')
        {
            board[x][y] = '*';//布置雷
            count++;
        }
    }
    //然后再将刚刚的区域消去
    for (int i = row - 1; i <= row + 1; i++)
    {
        for (int j = col - 1; j <= col + 1; j++)
        {
            board[i][j] = '#';//起到保护作用
        }
    }
}

//获取该格子周围雷的数量
int countMines(char board[ROW][COL], int row, int col)
{
    int count = 0;
    int i, j;
    for (i = row - 1; i <= row + 1; i++)
    {
        for (j = col - 1; j <= col + 1; j++)
        {
            if (i >= 0 && i < ROW && j >= 0 && j < COL && board[i][j] == '*')
            {
                count++;//如果有雷则+1，统计周围雷的总数
            }
        }
    }
    return count;//返回雷数
}

//将数据计算并加入棋盘
void InsertData(char board[ROW][COL],int row,int col)
{
    //便利整个棋盘
    for (row = 0; row < ROW; row++)
    {
        for (col = 0; col < COL; col++)
        {
            if (board[row][col] != '*' && board[row][col] != '\0')//判断是否为数字
            {
                int count = countMines(board, row, col);//获取旁边雷的数量
                if (count == 0)
                {
                    board[row][col] = '\0';//周围没雷的格子消去
                }
                else
                board[row][col] = count + '0';//周围有雷的格子加入数字
            }
        }
    }
}

//获取棋盘中字符a的数量
int Find_sign(char board[ROW][COL],char a)
{
    int count = 0;
    //遍历整个棋盘
    for (int row = 0; row < ROW; row++)
    {
        for (int col = 0; col < COL; col++)
        {
            if (board[row][col] == a)
            {
                count ++;//统计字符a的数量
            }
        }
    }
    return count;//返回数量
}

/* 判断游戏是否成功 
    @return:1表示成功
            0表示还未成功
*/
int Judge_func(char board[ROW][COL])
{
    //将雷全部标记完或者将其他空全部排完只剩雷则成功
    if (/* Find_sign(board,'X') == MINES && Find_sign(board,'#') == 0||Find_sign(board,'X')+ */Find_sign(board,'#') == MINES)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

//将内部棋盘可展示部分赋值给展示棋盘，并递归找空地
void revealCell(char board[ROW][COL], char displayBoard[ROW][COL], int row, int col)
{
    if (row < 0 || row >= ROW || col < 0 || col >= COL || displayBoard[row][col] != '#' || board[row][col] == '*') 
    {
        return;
    }
    //将输入点坐标对应的内部棋盘数据传递给展示棋盘
    displayBoard[row][col] = board[row][col];
    //如果为空地，则递归该指定坐标周围的八个格子，coner
    if (board[row][col] == '\0')
    {
        for (int i = row - 1; i <= row + 1; i++) 
        {
            for (int j = col - 1; j <= col + 1; j++) 
            {
                revealCell(board, displayBoard,i,j);
            }
        }
    }
}
