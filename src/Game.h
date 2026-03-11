// Game.h
#pragma once
#include "Utils.h"
using namespace sf;

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

// 文件操作函数
void loadHighScore();
void saveHighScore();