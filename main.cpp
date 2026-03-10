#include <SFML/Graphics.hpp>
#include <time.h>
using namespace sf;

// ========== 1. 常量定义 ==========
const int M = 20;              // 游戏区域行数
const int N = 10;              // 游戏区域列数
const int BLOCK_SIZE = 18;     // 方块大小（像素）
const int WINDOW_WIDTH = 450;   // 窗口宽度
const int WINDOW_HEIGHT = 416;  // 窗口高度
const float BASE_DELAY = 0.5f;  // 基础下落速度（秒）

// ========== 2. 方块形状数据 ==========
int blocks[7][4][4] = {
    // I (长条)
    {
        {4,5,6,7},  // 垂直 I
        {1,5,9,13}, // 水平 I
        {4,5,6,7},  // 同垂直
        {1,5,9,13}  // 同水平
    },
    // Z (之字形)
    {
        {0,1,5,6},  // 水平 Z
        {1,4,5,8},  // 垂直 Z
        {0,1,5,6},  // 同水平
        {1,4,5,8}   // 同垂直
    },
    // S (反之字)
    {
        {1,2,4,5},  // 水平 S
        {0,4,5,9},  // 垂直 S
        {1,2,4,5},  // 同水平
        {0,4,5,9}   // 同垂直
    },
    // T (T形)
    {
        {1,4,5,6},  // T 朝上
        {1,4,5,9},  // T 朝左
        {4,5,6,9},  // T 朝下
        {1,5,6,9}   // T 朝右
    },
    // L (L形)
    {
        {0,4,5,6},  // L 朝右
        {1,5,8,9},  // L 朝上
        {0,1,2,6},  // L 朝左
        {0,1,4,8}   // L 朝下
    },
    // J (J形)
    {
        {2,4,5,6},  // J 朝左
        {0,1,5,9},  // J 朝下
        {0,1,2,4},  // J 朝右
        {0,4,8,9}   // J 朝上
    },
    // O (田字)
    {
        {0,1,4,5},  // 田字
        {0,1,4,5},  // 旋转不变
        {0,1,4,5},
        {0,1,4,5}
    }
};

// ========== 3. 结构体定义 ==========
struct Point {
    int x, y;
};

// ========== 4. 全局变量 ==========
int field[M][N] = { 0 };        // 已固定的方块
int currentShape = 4;           // 当前方块的形状
int currentRotation = 0;        // 当前方块的旋转状态
Point currentPos = { 3, 0 };    // 当前方块的位置
int nextShape = 0;              // 下一个方块
bool gameOver = false;          // 游戏结束标志
int score = 0;                  // 当前分数
int highScore = 0;            // 最高分
float timer = 0.0f;             // 自动下落计时器
float delay = BASE_DELAY;       // 当前下落速度
bool paused = false;            // 暂停标志

// ========== 5. 函数声明 ==========
// 基础工具函数
void getBlockPositions(Point out[4], int shape, int rotation, Point pos);
bool isValidMove(int shape, int rotation, Point pos);

// 游戏逻辑函数
void fixCurrentBlock();
int clearLines();
void spawnNewBlock();
void resetGame();

// 游戏流程函数
void handleInput(RenderWindow& window);
void updateGame(float time);

// 文件操作函数(最高分)
void loadHighScore();
void saveHighScore();

// ========== 6. UI类 ==========
class UI {
private:
    Font& font;

    // 私有绘制函数
    void drawGameBoard(RenderWindow& window) {
        // 灰色底板
        RectangleShape board(Vector2f(N * BLOCK_SIZE, M * BLOCK_SIZE));
        board.setFillColor(Color(192, 192, 192));
        board.setPosition(0.f, 0.f);
        board.move(28.f, 31.f);
        window.draw(board);

        // 黑色边框
        RectangleShape border(Vector2f(N * BLOCK_SIZE, M * BLOCK_SIZE));
        border.setFillColor(Color::Transparent);
        border.setOutlineColor(Color::Black);
        border.setOutlineThickness(1.f);
        border.setPosition(0.f, 0.f);
        border.move(28.f, 31.f);
        window.draw(border);
    }

    void drawGrid(RenderWindow& window) {
        // 水平网格线
        for (int i = 0; i <= M; i++) {
            RectangleShape line(Vector2f(N * BLOCK_SIZE, 1.f));
            line.setFillColor(Color(150, 150, 150));
            line.setPosition(0.f, i * BLOCK_SIZE);
            line.move(28.f, 31.f);
            window.draw(line);
        }

        // 垂直网格线
        for (int j = 0; j <= N; j++) {
            RectangleShape line(Vector2f(1.f, M * BLOCK_SIZE));
            line.setFillColor(Color(150, 150, 150));
            line.setPosition(j * BLOCK_SIZE, 0.f);
            line.move(28.f, 31.f);
            window.draw(line);
        }
    }

    void drawFixedBlocks(RenderWindow& window) {
        for (int i = 0; i < M; i++) {
            for (int j = 0; j < N; j++) {
                if (field[i][j] == 0) continue;

                RectangleShape rect(Vector2f(BLOCK_SIZE, BLOCK_SIZE));
                rect.setFillColor(Color::Black);
                rect.setPosition(j * BLOCK_SIZE, i * BLOCK_SIZE);
                rect.move(28.f, 31.f);
                window.draw(rect);
            }
        }
    }

    void drawCurrentBlock(RenderWindow& window) {
        Point current[4];
        getBlockPositions(current, currentShape, currentRotation, currentPos);

        for (int i = 0; i < 4; i++) {
            int x = current[i].x;
            int y = current[i].y;

            if (x >= 0 && x < N && y >= 0 && y < M) {
                RectangleShape rect(Vector2f(BLOCK_SIZE, BLOCK_SIZE));
                rect.setFillColor(Color::Black);
                rect.setPosition(x * BLOCK_SIZE, y * BLOCK_SIZE);
                rect.move(28.f, 31.f);
                window.draw(rect);
            }
        }
    }

    void drawNextBlock(RenderWindow& window) {
        // 预览区域背景
        RectangleShape previewBg(Vector2f(80.f, 80.f));
        previewBg.setFillColor(Color(50, 50, 50));
        previewBg.setPosition(340, 50);
        window.draw(previewBg);

        // 绘制预览方块
        Point block[4];
        Point previewPos = { 0, 0 };
        getBlockPositions(block, nextShape, 0, previewPos);

        float scale = 0.7f;
        for (int i = 0; i < 4; i++) {
            RectangleShape rect(Vector2f(BLOCK_SIZE * scale, BLOCK_SIZE * scale));
            rect.setFillColor(Color::Black);
            rect.setPosition(350 + block[i].x * BLOCK_SIZE * scale,
                60 + block[i].y * BLOCK_SIZE * scale);
            window.draw(rect);
        }
    }

public:
    Text scoreLabel;
    Text scoreValue;
    Text highScoreLabel;
    Text highScoreValue;
    Text nextText;
    Text gameOverText;
    Text pauseText;

    UI(Font& f) : font(f) {
        // 分数标签
        scoreLabel = Text("Score", font, 20);
        scoreLabel.setFillColor(Color::White);
        scoreLabel.setPosition(340, 200);

        // 分数值
        scoreValue = Text("0", font, 24);
        scoreValue.setFillColor(Color::Yellow);
        scoreValue.setPosition(340, 230);

        // 最高分标签
        highScoreLabel = Text("Best", font, 20);
        highScoreLabel.setFillColor(Color::White);
        highScoreLabel.setPosition(340, 260);

        // 最高分数值
        highScoreValue = Text("0", font, 24);
        highScoreValue.setFillColor(Color(255, 215, 0));  // 金色
        highScoreValue.setPosition(340, 290);

        // 下一个方块标签
        nextText = Text("Next", font, 20);
        nextText.setFillColor(Color::White);
        nextText.setPosition(340, 20);

        // 游戏结束文字
        gameOverText = Text("GAME OVER", font, 40);
        gameOverText.setFillColor(Color::Red);
        gameOverText.setPosition(100, 180);

        // 暂停文字
        pauseText = Text("PAUSE", font, 40);
        pauseText.setFillColor(Color::Yellow);
        pauseText.setPosition(150, 180);
    }

    void updateScore(int newScore) {
        char scoreStr[16];
        sprintf_s(scoreStr, 16, "%d", newScore);
        scoreValue.setString(scoreStr);
    }

    void updateHighScore(int newHighScore) {
        char scoreStr[16];
        sprintf_s(scoreStr, 16, "%d", newHighScore);
        highScoreValue.setString(scoreStr);
    }

    // 统一的渲染函数
    void render(RenderWindow& window) {
        window.clear(Color::Black);

        // 绘制游戏区域
        drawGameBoard(window);
        drawGrid(window);
        drawFixedBlocks(window);
        drawCurrentBlock(window);

        // 绘制UI文字
        window.draw(scoreLabel);
        window.draw(scoreValue);
        window.draw(highScoreLabel);    // 新增
        window.draw(highScoreValue);
        window.draw(nextText);

        // 绘制下一个方块
        drawNextBlock(window);

        // 绘制游戏结束文字
        if (gameOver) {
            window.draw(gameOverText);
        }

        // 绘制暂停文字
        if (paused && !gameOver) {
            window.draw(pauseText);
        }

        window.display();
    }
};

// ========== 7. main函数 ==========
int main() {
    srand(time(0));
    RenderWindow window(VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "俄罗斯方块");

    Clock clock;

    // 加载字体
    Font font;
    font.loadFromFile("arial.ttf");

    // 创建UI（现在包含了所有绘制功能）
    UI ui(font);

    // 加载最高分
    loadHighScore();
    ui.updateHighScore(highScore);

    // 初始化游戏
    nextShape = rand() % 7;
    spawnNewBlock();

    while (window.isOpen()) {
        // 时间更新
        float time = clock.getElapsedTime().asSeconds();
        clock.restart();
        timer += time;

        // 事件处理
        handleInput(window);

        // 自动下落
        if (!gameOver && !paused && timer > delay) {
            updateGame(time);
        }

        // 渲染（现在直接调用ui.render）
        ui.updateScore(score);
        ui.updateHighScore(highScore);
        ui.render(window);
    }

    return 0;
}

// ========== 8. 函数实现 ==========

// 处理输入
void handleInput(RenderWindow& window) {
    Event e;
    while (window.pollEvent(e)) {
        if (e.type == Event::Closed)
            window.close();

        // R键重置（始终有效）
        if (e.type == Event::KeyPressed && e.key.code == Keyboard::R) {
            resetGame();
        }

        // P键暂停/继续游戏（在游戏未结束时有效）
        if (e.type == Event::KeyPressed && e.key.code == Keyboard::P && !gameOver) {
            paused = !paused;
        }

        // 游戏未结束时处理其他按键
        if (!gameOver && !paused && e.type == Event::KeyPressed) {
            Point newPos = currentPos;

            switch (e.key.code) {
            case Keyboard::Left:
                newPos.x--;
                if (isValidMove(currentShape, currentRotation, newPos))
                    currentPos = newPos;
                break;

            case Keyboard::Right:
                newPos.x++;
                if (isValidMove(currentShape, currentRotation, newPos))
                    currentPos = newPos;
                break;

            case Keyboard::Up: {
                int newRotation = (currentRotation + 1) % 4;
                if (isValidMove(currentShape, newRotation, currentPos))
                    currentRotation = newRotation;
                break;
            }

            case Keyboard::Down:
                newPos.y++;
                if (isValidMove(currentShape, currentRotation, newPos))
                    currentPos = newPos;
                break;

            case Keyboard::Space: {
                Point dropPos = currentPos;
                while (isValidMove(currentShape, currentRotation, dropPos))
                    dropPos.y++;
                dropPos.y--;
                if (dropPos.y != currentPos.y) {
                    currentPos = dropPos;
                    fixCurrentBlock();
                    clearLines();
                    spawnNewBlock();
                    timer = 0;
                }
                break;
            }
            }
        }
    }
}

// 更新游戏逻辑
void updateGame(float time) {
    if (timer > delay) {
        Point newPos = currentPos;
        newPos.y++;

        if (isValidMove(currentShape, currentRotation, newPos)) {
            currentPos = newPos;
        }
        else {
            fixCurrentBlock();
            clearLines();
            spawnNewBlock();
        }
        timer = 0;
    }
}

// 基础工具函数
void getBlockPositions(Point out[4], int shape, int rotation, Point pos) {
    for (int i = 0; i < 4; i++) {
        int num = blocks[shape][rotation][i];
        out[i].x = pos.x + (num % 4);
        out[i].y = pos.y + (num / 4);
    }
}

bool isValidMove(int shape, int rotation, Point pos) {
    Point block[4];
    getBlockPositions(block, shape, rotation, pos);

    for (int i = 0; i < 4; i++) {
        if (block[i].x < 0 || block[i].x >= N) return false;
        if (block[i].y >= M) return false;
        if (block[i].y >= 0 && field[block[i].y][block[i].x] != 0) return false;
    }
    return true;
}

// 游戏逻辑函数
void fixCurrentBlock() {
    Point current[4];
    getBlockPositions(current, currentShape, currentRotation, currentPos);

    for (int i = 0; i < 4; i++) {
        int x = current[i].x;
        int y = current[i].y;
        if (y >= 0 && y < M && x >= 0 && x < N) {
            field[y][x] = 1;
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
        case 1: score += 100; break;
        case 2: score += 300; break;
        case 3: score += 500; break;
        case 4: score += 800; break;
        }
        printf("Score: %d\n", score);

        // 检查是否破纪录
        if (score > highScore) {
            highScore = score;
            printf("新纪录！最高分: %d\n", highScore);
            // 这里不立即保存，等游戏结束时保存
        }
    }

    return linesCleared;
}

void spawnNewBlock() {
    currentShape = nextShape;
    currentRotation = 0;
    currentPos = { 3, 0 };
    nextShape = rand() % 7;

    if (!isValidMove(currentShape, currentRotation, currentPos)) {
        gameOver = true;
        printf("Game Over\n");
        saveHighScore();
    }
}

void resetGame() {
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            field[i][j] = 0;
        }
    }

    gameOver = false;
    currentShape = rand() % 7;
    currentRotation = 0;
    currentPos = { 3, 0 };
    nextShape = rand() % 7;
    score = 0;
    timer = 0;
    paused = false;

    loadHighScore();

    printf("游戏重新开始\n");
}

// ========== 文件操作函数 ==========
void loadHighScore() {
    FILE* file;
    errno_t  err = fopen_s(&file, "highscore.dat", "rb");
    if (err == 0 && file) {
        fread(&highScore, sizeof(int), 1, file);
        fclose(file);
        printf("加载最高分: %d\n", highScore);
    }
    else {
        highScore = 0;
        printf("没有历史最高分记录\n");
    }
}

void saveHighScore() {
    FILE* file;
    errno_t err = fopen_s(&file, "highscore.dat", "wb");
    if (err == 0 && file) {
        fwrite(&highScore, sizeof(int), 1, file);
        fclose(file);
        printf("保存最高分: %d\n", highScore);
    }
}


