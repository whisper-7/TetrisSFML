// UI.h
#pragma once
#include "Game.h"
using namespace sf;

class UI {
private:
    static UI* instance;
    Font& font;
    sf::Music backgroundMusic;
    sf::SoundBuffer lineClearBuffer;
    sf::Sound lineClearSound;
    sf::SoundBuffer rotateBuffer;
    sf::Sound rotateSound;
    sf::SoundBuffer moveBuffer;
    sf::Sound moveSound;
    sf::SoundBuffer dropBuffer;
    sf::Sound dropSound;
    sf::SoundBuffer gameStartBuffer;
    sf::Sound gameStartSound;
    sf::SoundBuffer gameOverBuffer;
    sf::Sound gameOverSound;

    void drawGameBoard(RenderWindow& window);
    void drawGrid(RenderWindow& window);
    void drawFixedBlocks(RenderWindow& window);
    void drawCurrentBlock(RenderWindow& window);
    void drawNextBlock(RenderWindow& window);

public:
    Text scoreLabel;
    Text scoreValue;
    Text highScoreLabel;
    Text highScoreValue;
    Text nextText;
    Text gameOverText;
    Text pauseText;
    Text startText;

    UI(Font& f);

    static UI* getInstance();
    static void playLineClearSound();
    static void playRotateSound();
    static void playMoveSound();
    static void playDropSound();
    static void playGameStartSound();
    static void playGameOverSound();
    static void pauseMusic();
    static void resumeMusic();
    static void stopMusic();

    void updateScore(int newScore);
    void updateHighScore(int newHighScore);
    void render(RenderWindow& window);
};