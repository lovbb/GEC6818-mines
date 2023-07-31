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

#define TOUCHPATH  ("/dev/input/event0") 
#define LCDPATH ("/dev/fb0")
#define LCDSIZE (800*480*4)

static int  lcd_fd    = -1;
static int *lcd_point = NULL;

/**
 * @brief LCD_INIT 初始化屏幕
 * @return int 成功返回0，失败返回-1
*/
int LCD_INIT()
{
    // 打开屏幕文件
    lcd_fd = open(LCDPATH,O_RDWR);
    if(lcd_fd == -1)
        return -1;
    
    lcd_point = mmap(NULL,LCDSIZE,PROT_READ|PROT_WRITE,MAP_SHARED,lcd_fd,0);
    if(lcd_point == MAP_FAILED)
    {
        close(lcd_fd);
        lcd_fd = -1;
        return -1;
    }
    return 0;
}

int LCD_UNINIT()
{
    if(lcd_fd == -1)
        return -1;
    munmap(lcd_point,LCDSIZE);
    close(lcd_fd);
    return 0;
}

/**
 * @brief DrawPixel 画点函数
 * @param h 像素点所在行
 * @param w 像素点所在列
 * @param color 像素点需要显示的颜色
*/
void DrawPixel(int h,int w,int color)
{
    // 约束一下坐标范围
    if(h>=0&&h<480&&w>=0&&w<800)
        *(lcd_point+h*800+w) = color;
}

//画棋盘线
void Drawboard_line(int color)
{
    int h = 0;
    int w = 1;
    //画竖线
    for (h = 1; h < 480; h++)
    {
        if (h % 40 == 0)
        {
            for (w = 0; w < 680; w++)
            {
                DrawPixel(h,w,color);
            }
        }
    }
    //画横线
    for (w = 0; w < 700; w++)
    {
        if (w % 40 == 0)
        {
            for (h = 0; h <= 440; h++)
            {
                DrawPixel(h,w,color);
            }
        }
    }
}

//画背景
void DrawBackgournd(int color)
{
    for(int h = 0;h < 480;h++)
    {
        for(int w = 0;w < 800;w++)
        {
            DrawPixel(h,w,color);
        }
    }
}

/* 打印图片
    @posx:图片x轴偏移量
    @posy:图片y轴偏移量
    @pic_path:图片名
*/
int DisplayBMPPicture(int posx,int posy,const char *pic_path)
{
    // 第一步：打开图片
    int pic_fd = open(pic_path,O_RDONLY);
    if(pic_fd == -1)
    {
        perror("Open bmp file failed");
        return 1;
    }

    // 第二步读取数据，判断文件类型
    unsigned char data[4]={0};
    read(pic_fd,data,2);

    // 判断是否为BMP文件
    if(data[0]!=0x42||data[1]!=0x4d)
    {
        puts("this picture not BMP file");
        close(pic_fd);
        return 1;
    }

    // 读取图片的偏移量
    lseek(pic_fd,0x0a,SEEK_SET);
    read(pic_fd,data,4);
    int offset = data[3]<<24|data[2]<<16|data[1]<<8|data[0];

    lseek(pic_fd,0x12,SEEK_SET);
    read(pic_fd,data,4);
    int width = data[3]<<24|data[2]<<16|data[1]<<8|data[0];

    read(pic_fd,data,4);
    int height = data[3]<<24|data[2]<<16|data[1]<<8|data[0];

    lseek(pic_fd,0x1C,SEEK_SET);
    read(pic_fd,data,2);
    int depth = data[1]<<8|data[0];

    // 如果图片的宽度不为4的倍数，那么图片显示将会异常。字节对齐引起
    int rand_num = 0; // 填充随机数
    if((width*(depth / 8))%4)
    {
        rand_num = 4 - ((width*(depth / 8))%4);
    }

    int real_width_bytes = (width*depth/8)+rand_num;

    // 像素数组的数据

    // 开辟了一个动态可变数组
    unsigned char *color=calloc(sizeof(char),real_width_bytes*abs(height));
    lseek(pic_fd,offset,SEEK_SET);
    read(pic_fd,color,real_width_bytes*abs(height));

    // 处理数据
    // 注意： BMP图片的像素数组对于三原色的存储是反着来的，也就是像素数组的第一个字节保存的是第一个像素点的Blue值

    unsigned char *color_p = color;
    // 显示整幅图像
    for(int h = 0;h < abs(height);h++)
    {
        for(int w = 0;w < width;w++)
        {
            unsigned char a,r,g,b;
            b =  *color_p++;
            g =  *color_p++;
            r =  *color_p++;
            a = (depth == 24)?0:*color_p++;
            //int color_var = color[2] << 16|color[1]<<8|color[0];
            int color_var = a<<24|r<<16|g<<8|b;
            DrawPixel(((height < 0)?h:height-1-h)+posy,w+posx,color_var);
        }
        
        // 更新指针
        color_p+= rand_num;
    }

    // 收尾工作
    free(color);
    close(pic_fd);
    return 0;
}

//显示棋盘
void Darw_board(int first,char board[ROW][COL])
{
    //最开始不显示结束游戏的图标
    if (first)
    {
        DisplayBMPPicture(680,5,"over.bmp");
    }
    for(int h = 0;h < ROW;h++)
    { 
        for(int w = 0; w < COL; w++)
        {
            //根据棋盘元素在对应位置打印相应的图片
            switch (board[h][w])
            {
            case '\0':
                DisplayBMPPicture(w*40,h*40,"0.bmp");
                break;
            case '1':
                DisplayBMPPicture(w*40,h*40,"1.bmp");
                break;
            case '2':
                DisplayBMPPicture(w*40,h*40,"2.bmp");
                break;
            case '3':
                DisplayBMPPicture(w*40,h*40,"3.bmp");
                break;
            case '4':
                DisplayBMPPicture(w*40,h*40,"4.bmp");
                break;
            case '5':
                DisplayBMPPicture(w*40,h*40,"5.bmp");
                break;
            case '6':
                DisplayBMPPicture(w*40,h*40,"6.bmp");
                break;
            case '7':
                DisplayBMPPicture(w*40,h*40,"7.bmp");
                break;
            case '8':
                DisplayBMPPicture(w*40,h*40,"8.bmp");
                break;
            case '*':
                DisplayBMPPicture(w*40,h*40,"mines.bmp");
                break;
            default:
                DisplayBMPPicture(w*40,h*40,"initial.bmp");
                break;
            }
        }
    }
    //画棋盘线
    Drawboard_line(0x000000);
}

//获取手指点击屏幕的坐标,超出范围返回1,未超出范围返回-1
void Get_XY(int *x,int *y)
{
    int x1 = -1,y1 = -1;//滑动开始坐标

    int fd = open("/dev/input/event0",O_RDWR);
    if(fd == -1)
    {
        printf("open error\n");
        return;
    }
    struct input_event ev;

    while(1)
    {
        int ret = read(fd,&ev,sizeof(ev));
        if(ret != sizeof(ev))
        {
            continue;
        }
        //将获取到的坐标值赋值给x1,y1
        if(ev.type == EV_ABS && ev.code == ABS_X)
        {
            x1 = ev.value*800/1024;
        }
        if(ev.type == EV_ABS && ev.code == ABS_Y)
        {
            y1 = ev.value*480/600;
        }
        //手指离开时才会满足if条件
        if(ev.type == EV_KEY && ev.code == BTN_TOUCH && ev.value == 0)
        {
            *x = x1 / 40;//用指针来更新传入函数的坐标
            *y = y1 / 40;
            close(fd);
            return;
        }
    }
}

//获取手指点击屏幕的坐标之后进行开始游戏的操作
void Start_game(char board[ROW][COL],char displayBoard[ROW][COL],int Y,int X)
{
    int first = 1;
    while(1)
    {
        //获取手指点击坐标
        Get_XY(&Y,&X);
        if (Y < 17 && X < 11)
        {
            //为了避免第一次就寄掉所以在第一次输入坐标之后再对内部棋盘进行落雷并插入数字
            if (first)//第一次选完点
            {
                DownMines(board,X,Y);//落雷

                InsertData(board,X,Y);//在内部棋盘插入数字

                first = 0;//之后输入坐标不再随机落雷与插数字
            }
            if (board[X][Y] == '*') //游戏以踩雷结束
            {
                DisplayBMPPicture(60,0,"FAIL.bmp");
                sleep(1);
                DrawBackgournd(0xffffff);
                Darw_board(1,board);//打印出内部棋盘
                break;
            }
            else
            {
                revealCell(board,displayBoard,X,Y);//扩散
            }
            //显示展示棋盘
            Darw_board(1,displayBoard);
        }
        //主动结束游戏
        if (Y > 17 && X <2)
        {
            DisplayBMPPicture(60,0,"FAIL.bmp");
            sleep(1);
            DrawBackgournd(0xffffff);
            Darw_board(1,board);//打印出内部棋盘
            break;
        }
        
        if (Judge_func(displayBoard))//雷被全部排完，成功通关
        {
            sleep(1);
            DisplayBMPPicture(0,0,"VICTORY.bmp");//展示成功通关图片
            sleep(1);
            DrawBackgournd(0xffffff);
            Darw_board(1,board);//打印出内部棋盘
            break;
        }
    }
    //出循环代表本局游戏结束，显示再来一局以及退出游戏程序的图片
    DisplayBMPPicture(680,80,"ag.bmp");
    DisplayBMPPicture(680,230,"exit.bmp");
    return;
}

//初始化界面
int Initial()
{
    int Y = -1;
    int X = -1;
    //展示界面图片
    DisplayBMPPicture(0,0,"start2.bmp");
    sleep(3);
    DisplayBMPPicture(0,0,"start.bmp");
    while(1)
    {   //获取点击坐标
        Get_XY(&Y,&X);
        //限制进入游戏区域
        if (Y < 13 && Y > 8 && X < 8 && X > 4)
        {
            return 1;
        }
    }
}

//重新开始游戏与退出游戏程序
int Restart()
{
    int X = -1;
    int Y = -1;
    while(1)
    {   //获取点击坐标
        Get_XY(&Y,&X);
        //限制重新开始的区域
        if (Y > 17 && X > 2 && X < 5)
        {

            return 1;
        }
        else if (Y >17 && X >5 && X <8)
        {
            return 0;
        }
    }
}
