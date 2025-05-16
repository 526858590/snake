#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>
#include <time.h>

// 常量定义
#define MAP_WIDTH 80   // 增加地图宽度
#define MAP_HEIGHT 25  // 增加地图高度
#define INIT_SNAKE_LENGTH 5
#define MAX_SNAKE_LENGTH 100

// 方向定义
#define UP 1
#define DOWN 2
#define LEFT 3
#define RIGHT 4

// 颜色定义
#define COLOR_SNAKE 14  // 黄色
#define COLOR_FOOD 12   // 红色
#define COLOR_WALL 15   // 白色
#define COLOR_TEXT 10   // 绿色

// 位置结构体
typedef struct {
    int x;
    int y;
} Position;

// 蛇结构体
typedef struct {
    Position pos[MAX_SNAKE_LENGTH];
    int length;
    int direction;
    int speed;
    int score;
} Snake;

// 全局变量
Snake snake;
Position food;
int highScore = 0;
int sleepTime = 200;

// 函数声明
void initConsole(void);
void gotoxy(int x, int y);
void setColor(int color);
void printSnake(void);
void welcomeToGame(void);
void createMap(void);
void scoreAndTips(void);
void fileOut(void);
void initSnake(void);
void createFood(void);
int biteSelf(void);
void cantCrossWall(void);
void speedUp(void);
void speedDown(void);
void snakeMove(void);
void keyboardControl(void);
void gameOver(void);
void explanation(void);
void saveScore(int score);

// 初始化控制台
void initConsole(void) {
    // 设置控制台代码页为GBK
    system("chcp 936");
    // 设置控制台窗口大小
    system("mode con cols=120 lines=35");
    system("title 贪吃蛇大作战");
}

// 光标定位
void gotoxy(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

// 设置颜色
void setColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

// 打印蛇标题
void printSnake(void) {
    system("cls");
    setColor(COLOR_SNAKE);
    printf("\n\n");
    printf("                  贪  吃  蛇  大  作  战\n");
    printf("                ========================\n");
    printf("                   Powered by C语言\n");
    printf("\n\n");
    setColor(COLOR_TEXT);
    printf("                  ┌──────────────────┐\n");
    printf("                  │    1. 开始游戏   │\n");
    printf("                  │    2. 游戏说明   │\n");
    printf("                  │    3. 退出游戏   │\n");
    printf("                  └──────────────────┘\n");
}

// 欢迎界面
void welcomeToGame(void) {
    char choice;
    while (1) {
        printSnake();
        choice = _getch();
        switch (choice) {
        case '1':
            system("cls");
            createMap();
            initSnake();
            createFood();
            keyboardControl();
            break;
        case '2':
            explanation();
            break;
        case '3':
            exit(0);
        default:
            continue;
        }
    }
}

// 创建地图
void createMap(void) {
    int i, j;
    setColor(COLOR_WALL);

    // 绘制上边框
    gotoxy(0, 0);
    printf("┌");
    for (i = 0; i < MAP_WIDTH - 2; i++)
        printf("─");
    printf("┐");

    // 绘制中间部分
    for (i = 1; i < MAP_HEIGHT - 1; i++) {
        gotoxy(0, i);
        printf("│");
        for (j = 0; j < MAP_WIDTH - 2; j++)
            printf(" ");
        printf("│");
    }

    // 绘制下边框
    gotoxy(0, MAP_HEIGHT - 1);
    printf("└");
    for (i = 0; i < MAP_WIDTH - 2; i++)
        printf("─");
    printf("┘");
}

// 显示分数和提示
void scoreAndTips(void) {
    gotoxy(MAP_WIDTH + 5, 3);
    printf("┌────────────────────┐");
    gotoxy(MAP_WIDTH + 5, 4);
    printf("│     游戏信息       │");
    gotoxy(MAP_WIDTH + 5, 5);
    printf("├────────────────────┤");
    gotoxy(MAP_WIDTH + 5, 6);
    printf("│ 当前得分: %-8d │", snake.score);
    gotoxy(MAP_WIDTH + 5, 7);
    printf("│ 历史最高: %-8d │", highScore);
    gotoxy(MAP_WIDTH + 5, 8);
    printf("├────────────────────┤");
    gotoxy(MAP_WIDTH + 5, 9);
    printf("│     操作说明       │");
    gotoxy(MAP_WIDTH + 5, 10);
    printf("│ ↑↓←→: 移动方向     │");
    gotoxy(MAP_WIDTH + 5, 11);
    printf("│ F1: 加速           │");
    gotoxy(MAP_WIDTH + 5, 12);
    printf("│ F2: 减速           │");
    gotoxy(MAP_WIDTH + 5, 13);
    printf("│ ESC: 退出          │");
    gotoxy(MAP_WIDTH + 5, 14);
    printf("├────────────────────┤");
    gotoxy(MAP_WIDTH + 5, 15);
    printf("│     游戏状态       │");
    gotoxy(MAP_WIDTH + 5, 16);
    printf("│ 蛇的长度: %-8d │", snake.length);
    gotoxy(MAP_WIDTH + 5, 17);
    printf("│ 当前速度: %-8d │", (400 - sleepTime) / 10);
    gotoxy(MAP_WIDTH + 5, 18);
    printf("└────────────────────┘");
}

// 读取最高分
void fileOut(void) {
    FILE* fp = fopen("save.txt", "r");
    if (fp) {
        fscanf(fp, "%d", &highScore);
        fclose(fp);
    }
}

// 初始化蛇
void initSnake(void) {
    int i;
    snake.length = INIT_SNAKE_LENGTH;
    snake.direction = RIGHT;
    snake.speed = 1;
    snake.score = 0;

    for (i = 0; i < snake.length; i++) {
        snake.pos[i].x = MAP_WIDTH / 3 - i * 2;
        snake.pos[i].y = MAP_HEIGHT / 2;
        gotoxy(snake.pos[i].x, snake.pos[i].y);
        setColor(COLOR_SNAKE);
        if (i == 0) {
            printf("★");  // 蛇头
        }
        else {
            printf("○");  // 蛇身
        }
    }
}

// 创建食物
void createFood(void) {
    int flag;
    int i;
    do {
        flag = 0;
        food.x = (rand() % ((MAP_WIDTH - 4) / 2)) * 2 + 2;
        food.y = rand() % (MAP_HEIGHT - 2) + 1;

        for (i = 0; i < snake.length; i++) {
            if (food.x == snake.pos[i].x && food.y == snake.pos[i].y) {
                flag = 1;
                break;
            }
        }
    } while (flag);

    gotoxy(food.x, food.y);
    setColor(COLOR_FOOD);
    printf("●");  // 食物
}

// 检测是否咬到自己
int biteSelf(void) {
    int i;
    for (i = 1; i < snake.length; i++) {
        if (snake.pos[0].x == snake.pos[i].x && snake.pos[0].y == snake.pos[i].y)
            return 1;
    }
    return 0;
}

// 检测是否撞墙
void cantCrossWall(void) {
    if (snake.pos[0].x == 0 || snake.pos[0].x == MAP_WIDTH - 1 ||
        snake.pos[0].y == 0 || snake.pos[0].y == MAP_HEIGHT - 1) {
        gameOver();
    }
}

// 加速
void speedUp(void) {
    if (sleepTime >= 50)
        sleepTime -= 10;
    snake.score += 2;
}

// 减速
void speedDown(void) {
    if (sleepTime <= 320)
        sleepTime += 30;
    if (snake.score >= 2)
        snake.score -= 2;
}

// 蛇的移动
void snakeMove(void) {
    int i;
    gotoxy(snake.pos[snake.length - 1].x, snake.pos[snake.length - 1].y);
    printf(" ");

    for (i = snake.length - 1; i > 0; i--) {
        snake.pos[i] = snake.pos[i - 1];
    }

    switch (snake.direction) {
    case UP:
        snake.pos[0].y--;
        break;
    case DOWN:
        snake.pos[0].y++;
        break;
    case LEFT:
        snake.pos[0].x -= 2;
        break;
    case RIGHT:
        snake.pos[0].x += 2;
        break;
    }

    // 更新蛇的显示
    for (i = 0; i < snake.length; i++) {
        gotoxy(snake.pos[i].x, snake.pos[i].y);
        setColor(COLOR_SNAKE);
        if (i == 0) {
            printf("★");  // 蛇头
        }
        else {
            printf("○");  // 蛇身
        }
    }

    if (snake.pos[0].x == food.x && snake.pos[0].y == food.y) {
        snake.length++;
        snake.score++;
        createFood();
        speedUp();
    }

    cantCrossWall();
    if (biteSelf()) {
        gameOver();
    }

    scoreAndTips();
    Sleep(sleepTime);
}

// 键盘控制
void keyboardControl(void) {
    while (1) {
        if (GetAsyncKeyState(VK_UP) && snake.direction != DOWN)
            snake.direction = UP;
        else if (GetAsyncKeyState(VK_DOWN) && snake.direction != UP)
            snake.direction = DOWN;
        else if (GetAsyncKeyState(VK_LEFT) && snake.direction != RIGHT)
            snake.direction = LEFT;
        else if (GetAsyncKeyState(VK_RIGHT) && snake.direction != LEFT)
            snake.direction = RIGHT;
        else if (GetAsyncKeyState(VK_F1))
            speedUp();
        else if (GetAsyncKeyState(VK_F2))
            speedDown();
        else if (GetAsyncKeyState(VK_ESCAPE))
            gameOver();

        snakeMove();
    }
}

// 游戏结束
void gameOver(void) {
    system("cls");
    setColor(COLOR_TEXT);
    gotoxy(MAP_WIDTH / 3, MAP_HEIGHT / 2);

    if (snake.score > highScore) {
        printf("恭喜你创造了新记录！得分：%d", snake.score);
        saveScore(snake.score);
    }
    else {
        printf("游戏结束！得分：%d（最高记录：%d）", snake.score, highScore);
    }

    gotoxy(MAP_WIDTH / 3, MAP_HEIGHT / 2 + 2);
    printf("1. 再玩一次");
    gotoxy(MAP_WIDTH / 3, MAP_HEIGHT / 2 + 3);
    printf("2. 退出游戏");

    while (1) {
        char choice = _getch();
        if (choice == '1') {
            system("cls");
            welcomeToGame();
        }
        else if (choice == '2') {
            exit(0);
        }
    }
}

// 游戏说明
void explanation(void) {
    system("cls");
    setColor(COLOR_TEXT);
    printf("\n\n");
    printf("  ┌────────────────────────────────────┐\n");
    printf("  │         贪吃蛇游戏说明            │\n");
    printf("  ├────────────────────────────────────┤\n");
    printf("  │                                    │\n");
    printf("  │  1. 使用方向键 ↑↓←→ 控制蛇的移动 │\n");
    printf("  │  2. 吃到食物(●) 可以增长并得分   │\n");
    printf("  │  3. 撞到墙壁或咬到自己会游戏结束 │\n");
    printf("  │  4. 按F1键可以加速，加速后得分翻倍│\n");
    printf("  │  5. 按F2键可以减速，但得分会减半 │\n");
    printf("  │  6. 按ESC键可以随时退出游戏      │\n");
    printf("  │                                    │\n");
    printf("  ├────────────────────────────────────┤\n");
    printf("  │      按任意键返回主菜单...        │\n");
    printf("  └────────────────────────────────────┘\n");
    _getch();
}

// 保存最高分
void saveScore(int score) {
    FILE* fp = fopen("save.txt", "w");
    if (fp) {
        fprintf(fp, "%d", score);
        fclose(fp);
        highScore = score;
    }
}

// 主函数
int main() {
    srand((unsigned)time(NULL));
    initConsole();
    fileOut();
    welcomeToGame();
    return 0;
}