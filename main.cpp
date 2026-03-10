#include <SFML/Graphics.hpp>
#include <time.h>
using namespace sf;

const int M = 20;  // 游戏区域行数
const int N = 10;  // 游戏区域列数
const float BLOCK_SIZE = 18.f;  // 方块大小（像素）
const int WINDOW_WIDTH = 450;   // 窗口宽度
const int WINDOW_HEIGHT = 416;  // 窗口高度
const float BASE_DELAY = 0.5f;  // 基础下落速度（秒）

int blocks[7][4][4] = {
    // I (长条)
    {
        {4,5,6,7},  // 垂直 I
        {1,5,9,13},   // 水平 I
        {4,5,6,7},  // 同垂直
        {1,5,9,13}    // 同水平
    },
    // Z (之字形)
    {
        {0,1,5,6},  // 水平 Z
        {1,4,5,8},   // 垂直 Z
        {0,1,5,6},  // 同水平
        {1,4,5,8}    // 同垂直
    },
    // S (反之字)
    {
        {1,2,4,5},   // 水平 S
        {0,4,5,9},   // 垂直 S
        {1,2,4,5},   // 同水平
        {0,4,5,9}    // 同垂直
    },
    // T (T形)
    {
        {1,4,5,6},  // T 朝上
        {1,4,5,9},  // T 朝左
        {4,5,6,9},  // T 朝下
        {1,5,6,9}    // T 朝右
    },
    // L (L形)
    {
        {0,4,5,6}, // L 朝右
        {1,5,8,9},   // L 朝上
        {0,1,2,6},   // L 朝左
        {0,1,4,8}    // L 朝下
    },
    // J (J形)
    {
        {2,4,5,6}, // J 朝左
        {0,1,5,9},   // J 朝下
        {0,1,2,4},   // J 朝右
        {0,4,8,9}   // J 朝上
    },
    // O (田字)
    {
        {0,1,4,5},   // 田字
        {0,1,4,5},   // 旋转不变
        {0,1,4,5},
        {0,1,4,5}
    }
};


struct Point {
    int x, y;
};

// ============ 游戏状态 ============
int field[M][N] = { 0 };        // 已固定的方块
int currentShape = 4;           // 当前方块的形状
int currentRotation = 0;        // 当前方块的旋转状态
Point currentPos = { 3, 0 };    // 当前方块的位置
int nextShape = 0;              // 下一个方块
bool gameOver = false;          // 游戏结束标志
int score = 0;                  // 当前分数
float timer = 0.0f;             // 自动下落计时器
float delay = BASE_DELAY;             // 当前下落速度


// ========== 基础工具函数 ==========
void getBlockPositions(Point out[4], int shape, int rotation, Point pos) {
    for (int i = 0; i < 4; i++) {
        int num = blocks[shape][rotation][i];
        out[i].x = pos.x + (num % 4);  // 列偏移
        out[i].y = pos.y + (num / 4);  // 行偏移
    }
}
bool isValidMove(int shape, int rotation, Point pos) {
    Point block[4];
    getBlockPositions(block, shape, rotation, pos);

    for (int i = 0; i < 4; i++) {
        // 超出左右边界
        if (block[i].x < 0 || block[i].x >= N)
            return false;
        // 超出底部边界
        if (block[i].y >= M)
            return false;
        // 与已固定方块重叠（忽略顶部的负行）
        if (block[i].y >= 0 && field[block[i].y][block[i].x] != 0)
            return false;
    }
    return true;
}


// ========== 游戏逻辑函数 ==========

void fixCurrentBlock() {
    Point current[4];
    getBlockPositions(current, currentShape, currentRotation, currentPos);
    for (int i = 0; i < 4; i++) {
        int x = current[i].x;
        int y = current[i].y;
        if (y >= 0 && y < M && x >= 0 && x < N) {
            field[y][x] = 1; // 标记黑色
        }
    }
}
int clearLines() {
    int linesCleared = 0;
    for (int i = M - 1; i >= 0; i--) {
        bool full = true;
        for (int j = 0; j < N; j++) {
            if (field[i][j] == 0) {
                full = false;
                break;
            }
        }
        if (full) {
            linesCleared++;
            for (int k = i; k > 0; k--) {
                for (int j = 0; j < N; j++)
                    field[k][j] = field[k - 1][j];
            }
            for (int j = 0; j < N; j++) {
                field[0][j] = 0;
            }
            i++;
        }
    }
    if (linesCleared > 0) {
        switch (linesCleared) {
        case 1:score += 100; break;
        case 2:score += 300; break;
        case 3:score += 500; break;
        case 4:score += 800; break;
        }
        printf("Score: %d\n", score);
    }
    return linesCleared;
}
void spawnNewBlock() {
    currentShape = nextShape;
    currentRotation = 0;
    currentPos = { 3, 0 };  // 顶部中间

    nextShape = rand() % 7;

    if (!isValidMove(currentShape, currentRotation, currentPos)) {
        gameOver = true;
        printf("Game Over\n");
        // 可以在这里暂停游戏或重新开始
    }
}
void resetGame() {
    // 清空游戏区域
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            field[i][j] = 0;
        }
    }

    // 重置游戏状态
    gameOver = false;
    currentShape = rand() % 7;
    currentRotation = 0;
    currentPos = { 3, 0 };
    nextShape = rand() % 7;

    // 重置分数
    score = 0;

    // 重置计时器
    timer = 0;

    printf("游戏重新开始，分数已重置\n");
}


// ========== 绘制函数 ==========
void drawGameBoard(RenderWindow& window) {
    // 灰色底板
    RectangleShape board(Vector2f(N * BLOCK_SIZE, M * BLOCK_SIZE));
    board.setFillColor(Color(192, 192, 192, 192));
    board.setPosition(0.f, 0.f);
    board.move(28.f, 31.f);
    window.draw(board);
    
    // 黑色边框（可选，如果你想要边框的话）
    RectangleShape border(Vector2f(N * 18.f, M * 18.f));
    border.setFillColor(Color::Transparent);
    border.setOutlineColor(Color::Black);
    border.setOutlineThickness(1.f);
    border.setPosition(0.f, 0.f);
    border.move(28.f, 31.f);
    window.draw(border);
}

void drawGrid(RenderWindow& window) {
    for (int i = 0; i <= M; i++) {
        RectangleShape line(Vector2f(N * 18.f, 1.f));  // 水平线
        line.setFillColor(Color(200, 200, 200));          // 深灰色
        line.setPosition(0.f, i * 18.f);
        line.move(28.f, 31.f);
        window.draw(line);
    }

    for (int j = 0; j <= N; j++) {
        RectangleShape line(Vector2f(1.f, M * 18.f));  // 垂直线
        line.setFillColor(Color(50, 50, 50));
        line.setPosition(j * 18.f, 0.f);
        line.move(28.f, 31.f);
        window.draw(line);
    }
}

void drawFixedBlocks(RenderWindow& window) {
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            if (field[i][j] == 0) continue;

            RectangleShape rect(Vector2f(18.f, 18.f));
            rect.setFillColor(Color::Black);
            rect.setPosition(j * 18.f, i * 18.f);
            rect.move(28.f, 31.f);  // 偏移让方块在窗口中间
            window.draw(rect);
        }
    }
}

void drawCurrentBlock(RenderWindow& window, int shape, int rotation, Point pos) {
    Point current[4];
    getBlockPositions(current, currentShape, currentRotation, currentPos);

    for (int i = 0; i < 4; i++) {
        int x = current[i].x;
        int y = current[i].y;

        // 只绘制在屏幕范围内的
        if (x >= 0 && x < N && y >= 0 && y < M) {
            RectangleShape rect(Vector2f(18.f, 18.f));
            rect.setFillColor(Color::Black);
            rect.setPosition(x * 18.f, y * 18.f);
            rect.move(28.f, 31.f);  // 实现游戏逻辑到画面显示的转移
            window.draw(rect);
        }
    }
}

void drawNextBlock(RenderWindow& window, int shape) {
    // 预览区域背景（可选）
    RectangleShape previewBg(Vector2f(80.f, 80.f));
    previewBg.setFillColor(Color(50, 50, 50));  // 深灰色背景
    previewBg.setPosition(340, 50);  // 放在右侧
    window.draw(previewBg);

    // 绘制预览方块
    Point block[4];
    Point previewPos = { 0, 0 };  // 预览区域内的相对位置

    // 用旋转状态0来预览
    getBlockPositions(block, shape, 0, previewPos);

    float scale = 0.7f;  // 预览方块缩小一点
    for (int i = 0; i < 4; i++) {
        RectangleShape rect(Vector2f(18.f * scale, 18.f * scale));
        rect.setFillColor(Color::Black);  // 和你现在的方块颜色一致

        // 计算预览位置：从(350, 60)开始
        rect.setPosition(350 + block[i].x * 18.f * scale,
            60 + block[i].y * 18.f * scale);
        window.draw(rect);
    }
}



int main() {
    srand(time(0));
    RenderWindow window(VideoMode(450, 416), "俄罗斯方块");

    Clock clock;        // 计时器

    nextShape = rand() % 7;
    spawnNewBlock();

    Font font;
    font.loadFromFile("arial.ttf");
    // 分数板
    Text scoreLabel("Score", font, 20);
    scoreLabel.setFillColor(Color::White);
    scoreLabel.setPosition(340, 200);
    // 分数值
    Text scoreValue("0", font, 24);
    scoreValue.setFillColor(Color::Yellow);
    scoreValue.setPosition(340, 230);
    // 下一个方块的文字标签显示
    Text nextText("Next", font, 20);
    nextText.setFillColor(Color::White);
    nextText.setPosition(340, 20);
    
    // 游戏结束文字
    Text gameOverText("GAME OVER", font, 40);
    gameOverText.setFillColor(Color::Red);
    gameOverText.setPosition(100, 180);

    while (window.isOpen()) {
        // 计算时间差
        float time = clock.getElapsedTime().asSeconds();
        clock.restart();
        timer += time;

        Event e;
        while (window.pollEvent(e)) {
            if (e.type == Event::Closed)
                window.close();

            // R键始终有效（不管游戏是否结束）
            if (e.type == Event::KeyPressed && e.key.code == Keyboard::R) {
                resetGame();
            }

            if (!gameOver && e.type == Event::KeyPressed) {
                Point newPos = currentPos;

                switch (e.key.code) {
                    case Keyboard::Left: {
                        newPos.x--;
                        if (isValidMove(currentShape, currentRotation, newPos))
                            currentPos = newPos;
                        break;
                    }
                    case Keyboard::Right: {
                        newPos.x++;
                        if (isValidMove(currentShape, currentRotation, newPos))
                            currentPos = newPos;
                        break;
                    }
                    case Keyboard::Up: {
                        int newRotation = (currentRotation + 1) % 4;    //下一个旋转状态
                        if (isValidMove(currentShape, newRotation, currentPos)) {
                            currentRotation = newRotation;
                        }
                        break;
                    }
                    case Keyboard::Down: {
                        newPos.y++;
                        if (isValidMove(currentShape, currentRotation, newPos))
                            currentPos = newPos;
                        break;
                    }
                    case Keyboard::Space: {
                        Point newPos = currentPos;
                        while (isValidMove(currentShape, currentRotation, newPos)) {
                            newPos.y++;
                        }
                        newPos.y--;
                        if (newPos.y != currentPos.y) {
                            currentPos = newPos;
                            fixCurrentBlock();
                            clearLines();
                            spawnNewBlock();
                            timer = 0;
                        }
                    }
                }
            }
        }

        // 自动下落逻辑
        if (!gameOver && timer > delay) {
            Point newPos = currentPos;
            newPos.y++;

            if (isValidMove(currentShape, currentRotation, newPos)) {
                currentPos = newPos;
            }else{
                // 固定当前方块
                fixCurrentBlock();

                // 消除满行
                int linesCleared = clearLines();
                if (linesCleared > 0) {
                    printf("消除 %d 行！\n",linesCleared);
                }
                // 生成新方块
                spawnNewBlock();
            }
            timer = 0;
        }

        window.clear(Color::Black);

        // 绘制游戏区域边框
        drawGameBoard(window);
        // 绘制网格线
        drawGrid(window);
        // 绘制已固定的方块
        drawFixedBlocks(window);
        // 绘制当前方块
        drawCurrentBlock(window, currentShape, currentRotation, currentPos);


        // 更新分数显示
        char scoreStr[16];
        sprintf_s(scoreStr, "%d", score);
        scoreValue.setString(scoreStr);

        RectangleShape testDot(Vector2f(5, 5));
        testDot.setFillColor(Color::Red);
        testDot.setPosition(340, 50);
        window.draw(testDot);

        // 绘制分数
        window.draw(scoreLabel);
        window.draw(scoreValue);

        // 下一个方块的文字标签
        window.draw(nextText);
        // 绘制下一个方块预览 
        drawNextBlock(window, nextShape);

        if (gameOver) {
            window.draw(gameOverText);
        }

        window.display();
    }

    return 0;
}