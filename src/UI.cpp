// UI.cpp
#include "UI.h"
using namespace sf;

UI* UI::instance = nullptr;

UI::UI(Font& f) : font(f) {
    instance = this;

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
    highScoreValue.setFillColor(Color(255, 215, 0));
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

    // 开始提示文字
    startText = Text("Press ANY KEY to Start", font, 30);
    startText.setFillColor(Color::White);
    startText.setPosition(80, 200);

    // 加载背景音乐
    if (!backgroundMusic.openFromFile("assets/music/background.ogg")) {
        printf("❌ 加载背景音乐失败\n");
    }
    else {
        backgroundMusic.setLoop(true);
        backgroundMusic.setVolume(30.f);
        backgroundMusic.play();
    }

    // 加载音效
    auto loadSound = [&](sf::SoundBuffer& buffer, sf::Sound& sound, const char* path, float vol) {
        if (buffer.loadFromFile(path)) {
            sound.setBuffer(buffer);
            sound.setVolume(vol);
            printf("✅ 加载: %s\n", path);
        }
        else {
            printf("❌ 加载失败: %s\n", path);
        }
    };

    loadSound(lineClearBuffer, lineClearSound, "assets/sounds/line_clear.wav", 50.f);
    loadSound(rotateBuffer, rotateSound, "assets/sounds/rotate.wav", 40.f);
    loadSound(moveBuffer, moveSound, "assets/sounds/move.wav", 30.f);
    loadSound(dropBuffer, dropSound, "assets/sounds/drop.wav", 45.f);
    loadSound(gameStartBuffer, gameStartSound, "assets/sounds/game_start.wav", 50.f);
    loadSound(gameOverBuffer, gameOverSound, "assets/sounds/game_over.wav", 50.f);
}

// 静态方法
UI* UI::getInstance() { return instance; }
void UI::playLineClearSound() { if (instance) instance->lineClearSound.play(); }
void UI::playRotateSound() { if (instance) instance->rotateSound.play(); }
void UI::playMoveSound() { if (instance) instance->moveSound.play(); }
void UI::playDropSound() { if (instance) instance->dropSound.play(); }
void UI::playGameStartSound() { if (instance) instance->gameStartSound.play(); }
void UI::playGameOverSound() { if (instance) instance->gameOverSound.play(); }
void UI::pauseMusic() { if (instance) instance->backgroundMusic.pause(); }
void UI::resumeMusic() { if (instance) instance->backgroundMusic.play(); }
void UI::stopMusic() { if (instance) instance->backgroundMusic.stop(); }

// 公共方法
void UI::updateScore(int newScore) {
    char scoreStr[16];
    sprintf_s(scoreStr, 16, "%d", newScore);
    scoreValue.setString(scoreStr);
}

void UI::updateHighScore(int newHighScore) {
    char scoreStr[16];
    sprintf_s(scoreStr, 16, "%d", newHighScore);
    highScoreValue.setString(scoreStr);
}

// 绘制函数
void UI::drawGameBoard(RenderWindow& window) {
    RectangleShape board(Vector2f(N * BLOCK_SIZE, M * BLOCK_SIZE));
    board.setFillColor(Color(192, 192, 192));
    board.setPosition(0.f, 0.f);
    board.move(28.f, 31.f);
    window.draw(board);

    RectangleShape border(Vector2f(N * BLOCK_SIZE, M * BLOCK_SIZE));
    border.setFillColor(Color::Transparent);
    border.setOutlineColor(Color::Black);
    border.setOutlineThickness(1.f);
    border.setPosition(0.f, 0.f);
    border.move(28.f, 31.f);
    window.draw(border);
}

void UI::drawGrid(RenderWindow& window) {
    for (int i = 0; i <= M; i++) {
        RectangleShape line(Vector2f(N * BLOCK_SIZE, 1.f));
        line.setFillColor(Color(150, 150, 150));
        line.setPosition(0.f, i * BLOCK_SIZE);
        line.move(28.f, 31.f);
        window.draw(line);
    }
    for (int j = 0; j <= N; j++) {
        RectangleShape line(Vector2f(1.f, M * BLOCK_SIZE));
        line.setFillColor(Color(150, 150, 150));
        line.setPosition(j * BLOCK_SIZE, 0.f);
        line.move(28.f, 31.f);
        window.draw(line);
    }
}

void UI::drawFixedBlocks(RenderWindow& window) {
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

void UI::drawCurrentBlock(RenderWindow& window) {
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

void UI::drawNextBlock(RenderWindow& window) {
    RectangleShape previewBg(Vector2f(80.f, 80.f));
    previewBg.setFillColor(Color(50, 50, 50));
    previewBg.setPosition(340, 50);
    window.draw(previewBg);

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

void UI::render(RenderWindow& window) {
    window.clear(Color::Black);

    drawGameBoard(window);
    drawGrid(window);
    drawFixedBlocks(window);
    if (gameStarted) {
        drawCurrentBlock(window);
    }

    window.draw(scoreLabel);
    window.draw(scoreValue);
    window.draw(highScoreLabel);
    window.draw(highScoreValue);
    window.draw(nextText);
    if (!gameStarted) {
        window.draw(startText);
    }
    drawNextBlock(window);

    if (gameOver) {
        window.draw(gameOverText);
    }
    if (paused && !gameOver) {
        window.draw(pauseText);
    }
    window.display();
}