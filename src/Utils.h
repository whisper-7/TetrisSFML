// Utils.h
#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
using namespace sf;

// ========== 常量定义 ==========
const int M = 20;
const int N = 10;
const int BLOCK_SIZE = 18;
const int WINDOW_WIDTH = 450;
const int WINDOW_HEIGHT = 416;
const float BASE_DELAY = 0.5f;

// ========== 结构体定义 ==========
struct Point {
    int x, y;
};

// ========== 全局变量声明 ==========
extern int field[M][N];
extern int currentShape;
extern int currentRotation;
extern Point currentPos;
extern int nextShape;
extern bool gameOver;
extern int score;
extern int highScore;
extern float timer;
extern float delay;
extern bool paused;
extern bool gameStarted;