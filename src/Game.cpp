// Game.cpp
#include "Game.h"
#include "UI.h"
using namespace sf;

// ========== 方块形状数据 ==========
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

// ========== 全局变量定义 ==========
int field[M][N] = { 0 };
int currentShape = 4;
int currentRotation = 0;
Point currentPos = { 3, 0 };
int nextShape = 0;
bool gameOver = false;
int score = 0;
int highScore = 0;
float timer = 0.0f;
float delay = BASE_DELAY;
bool paused = false;
bool gameStarted = false;

// ========== 函数实现 ==========
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
        UI::playLineClearSound();
        switch (linesCleared) {
        case 1: score += 100; break;
        case 2: score += 300; break;
        case 3: score += 500; break;
        case 4: score += 800; break;
        }
        printf("Score: %d\n", score);
        if (score > highScore) {
            highScore = score;
            printf("新纪录！最高分: %d\n", highScore);
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
        gameStarted = false;
        printf("Game Over\n");
        UI::stopMusic();
        UI::playGameOverSound();
        saveHighScore();
    }
}

void resetGame() {
    saveHighScore();
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            field[i][j] = 0;
        }
    }
    gameOver = false;
    gameStarted = false;
    currentShape = rand() % 7;
    currentRotation = 0;
    currentPos = { 3, 0 };
    nextShape = rand() % 7;
    score = 0;
    timer = 0;
    paused = false;
    loadHighScore();
    UI::resumeMusic();
    UI::playGameStartSound();
    printf("游戏重新开始\n");
}

void handleInput(RenderWindow& window) {
    Event e;
    while (window.pollEvent(e)) {
        if (e.type == Event::Closed)
            window.close();

        if (!gameStarted && e.type == Event::KeyPressed) {
            gameStarted = true;
            printf("游戏开始！\n");
            continue;
        }
        if (e.type == Event::KeyPressed && e.key.code == Keyboard::R) {
            resetGame();
            continue;
        }
        if (e.type == Event::KeyPressed && e.key.code == Keyboard::P && !gameOver) {
            paused = !paused;
            if (paused) {
                UI::pauseMusic();
            }
            else {
                UI::resumeMusic();
            }
            continue;
        }
        if (gameStarted && !gameOver && !paused && e.type == Event::KeyPressed) {
            Point newPos = currentPos;
            switch (e.key.code) {
            case Keyboard::Left:
                newPos.x--;
                if (isValidMove(currentShape, currentRotation, newPos)) {
                    currentPos = newPos;
                    UI::playMoveSound();
                }
                break;
            case Keyboard::Right:
                newPos.x++;
                if (isValidMove(currentShape, currentRotation, newPos)) {
                    currentPos = newPos;
                    UI::playMoveSound();
                }
                break;
            case Keyboard::Up: {
                int newRotation = (currentRotation + 1) % 4;
                if (isValidMove(currentShape, newRotation, currentPos)) {
                    currentRotation = newRotation;
                    UI::playRotateSound();
                }
                break;
            }
            case Keyboard::Down:
                newPos.y++;
                if (isValidMove(currentShape, currentRotation, newPos)) {
                    currentPos = newPos;
                    UI::playMoveSound();
                }
                break;
            case Keyboard::Space: {
                Point dropPos = currentPos;
                while (isValidMove(currentShape, currentRotation, dropPos))
                    dropPos.y++;
                dropPos.y--;
                if (dropPos.y != currentPos.y) {
                    currentPos = dropPos;
                    UI::playDropSound();
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

void loadHighScore() {
    FILE* file;
    errno_t err = fopen_s(&file, "highscore.dat", "rb");
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