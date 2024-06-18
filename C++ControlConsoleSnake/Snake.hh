#include <iostream>
#include <windows.h>
#include <conio.h>
using namespace std;

// 定义地图的高度和宽度
#define H 24
#define W 68

// 方向数组，表示上、右、下、左
const int dir[4][2] = {
    {-1, 0}, // 上
    {0, 1},
    {1, 0},
    {0, -1}};

// 枚举类型，定义地图上的空格和食物
enum BlockType
{
    EMPTY = 0,
    FOOD = 1,
};

// 定义地图结构体
struct Map
{
    BlockType data[H][W];
    bool hasFood;
};

// 定义位置结构体
struct Pos
{
    int x;
    int y;
};

// 定义蛇结构体
struct Snake
{
    Pos snake[H * W];
    int snakeDir;      // 蛇的当前方向
    int snakeLength;   // 蛇的长度
    int lestMoveTime;  // 上次移动的时间
    int moveFrequency; // 蛇的移动频率
};

// 函数声明
void hideCursor();
void drawMap(Map *map);
void initMap(Map *map);
void initSnake(Snake *snk);
void drawSnake(Snake *snk);
bool checkSnakeMove(Snake *snk, Map *map);
bool domove(Snake *snk, Map *map);
void moveSnake(Snake *snk);
bool checkOutOfBound(Pos p);
void drawUnit(Pos p, const char unit[]);
void checkChangeDir(Snake *snk);
void checkFoodGenerate(Snake *snk, Map *map);
void checkEatFood(Snake *snk, Pos tail, Map *map);

// 绘制地图边框和内部
void drawMap(Map *map)
{
    //  ┌─┐││└─┘■
    cout << "┌";
    for (int x = 0; x < W; ++x)
    {
        cout << "─";
    }
    cout << "┐" << endl;
    for (int y = 1; y < H; ++y)
    {
        cout << "│";
        for (int x = 0; x < W; ++x)
        {
            if (map->data[y][x] == BlockType::EMPTY)
                cout << " ";
        }
        cout << "│" << endl;
    }
    cout << "└";
    for (int x = 0; x < W; ++x)
    {
        cout << "─";
    }
    cout << "┘" << endl;
}

// 初始化地图，设置所有位置为空
void initMap(Map *map)
{
    for (int y = 0; y < H; ++y)
    {
        for (int x = 0; x < W; ++x)
        {
            map->data[y][x] = BlockType::EMPTY;
        }
    }
    map->hasFood = false;
}

// 初始化蛇的位置和方向
void initSnake(Snake *snk)
{
    snk->snakeLength = 3;
    snk->snakeDir = 1;
    snk->snake[0] = {W / 2, H / 2};
    snk->snake[1] = {W / 2 - 1, H / 2};
    snk->snake[2] = {W / 2 - 2, H / 2};
    snk->lestMoveTime = GetTickCount();
    snk->moveFrequency = 100;
}

// 在指定位置绘制单元（例如食物或蛇的一段）
void drawUnit(Pos p, const char unit[])
{
    COORD coord;
    HANDLE hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    coord.X = p.x + 1;
    coord.Y = p.y + 1;
    SetConsoleCursorPosition(hOutput, coord);
    cout << unit;
}

// 绘制蛇的当前形态
void drawSnake(Snake *snk)
{
    for (int n = 0; n < snk->snakeLength; ++n)
        drawUnit(snk->snake[n], "■");
}

// 隐藏控制台光标
void hideCursor()
{
    CONSOLE_CURSOR_INFO cursor_info;
    cursor_info.bVisible = FALSE;
    cursor_info.dwSize = 1;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursor_info);
}

// 检查蛇是否可以移动，并且执行移动
bool checkSnakeMove(Snake *snk, Map *map)
{
    int curTime = GetTickCount();
    if (curTime - snk->lestMoveTime > snk->moveFrequency)
    {
        if (false == domove(snk, map))
            return false;
        snk->lestMoveTime = curTime;
    }
    return true;
}

// 执行蛇的移动逻辑
bool domove(Snake *snk, Map *map)
{
    Pos tail = snk->snake[snk->snakeLength - 1];
    drawUnit(tail, " ");
    moveSnake(snk);
    if (checkOutOfBound(snk->snake[0]))
        return false;
    checkEatFood(snk, tail, map);
    drawUnit(snk->snake[0], "■");
    return true;
}

// 移动蛇到新的位置
void moveSnake(Snake *snk)
{
    for (int n = snk->snakeLength - 1; n >= 1; --n)
        snk->snake[n] = snk->snake[n - 1];

    snk->snake[0].y += dir[snk->snakeDir][0];
    snk->snake[0].x += dir[snk->snakeDir][1];
}

// 检查位置是否超出地图边界
bool checkOutOfBound(Pos p)
{
    return (p.x == 0 || p.x == W + 1 || p.y == 0 || p.y == H + 1);
}

// 检查并改变蛇的移动方向
void checkChangeDir(Snake *snk)
{
    if (_kbhit())
    {
        switch (_getch())
        {
        case 'w':
            if (snk->snakeDir != 2)
                snk->snakeDir = 0;
            break;
        case 'd':
            if (snk->snakeDir != 3)
                snk->snakeDir = 1;
            break;
        case 's':
            if (snk->snakeDir != 0)
                snk->snakeDir = 2;
            break;
        case 'a':
            if (snk->snakeDir != 1)
                snk->snakeDir = 3;
            break;
        default:
            break;
        }
    }
}

// 生成食物的逻辑
void checkFoodGenerate(Snake *snk, Map *map)
{
    if (false == map->hasFood)
    {
        while (1)
        {
            int x = rand() % W;
            int y = rand() % H;
            int i = 0;
            while (i < snk->snakeLength)
            {
                if (x == snk->snake[i].x && y == snk->snake[i].y)
                    break;
                ++i;
            }
            if (i == snk->snakeLength)
            {
                map->data[y][x] = BlockType::FOOD;
                map->hasFood = true;
                drawUnit({x, y}, "●");
                return;
            }
        }
    }
}

// 检查蛇是否吃到食物，并更新蛇的长度和地图状态
void checkEatFood(Snake *snk, Pos tail, Map *map)
{
    Pos head = snk->snake[0];
    if (map->data[head.y][head.x] == BlockType::FOOD)
    {
        snk->snake[snk->snakeLength++] = tail;
        map->data[head.y][head.x] = BlockType::EMPTY;
        map->hasFood = false;
        drawUnit(tail, "■");
    }
}

// 游戏启动函数
void qidong()
{
    // system("cls");
    hideCursor();
    Map map;
    Snake snk;
    initMap(&map);
    initSnake(&snk);
    drawMap(&map);
    drawSnake(&snk);
    // 游戏主循环
    while (1)
    {
        checkChangeDir(&snk);
        if (checkSnakeMove(&snk, &map) == false)
            break;
        // 生成食物
        checkFoodGenerate(&snk, &map);
    }
    drawUnit({W / 2 - 4, H / 2}, "游戏结束");
    // 游戏结束，显示结束信息
    while (1)
    {
    }
}