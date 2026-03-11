// main.cpp
#include "UI.h"
using namespace sf;

int main() {
    srand(time(0));
    RenderWindow window(VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "俄罗斯方块");

    Clock clock;

    Font font;
    font.loadFromFile("assets/fonts/arial.ttf");

    UI ui(font);
    UI::playGameStartSound();

    loadHighScore();
    ui.updateHighScore(highScore);

    nextShape = rand() % 7;
    spawnNewBlock();

    while (window.isOpen()) {
        float time = clock.getElapsedTime().asSeconds();
        clock.restart();
        timer += time;

        handleInput(window);

        if (gameStarted && !gameOver && !paused) {
            updateGame(time);
        }

        ui.updateScore(score);
        ui.updateHighScore(highScore);
        ui.render(window);
    }

    return 0;
}