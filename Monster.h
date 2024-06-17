#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>


class Monster {
public:
    Monster(int hp, double speed, int baselineWaitTime, int baselineMoveTime, std::pair<int, int> position, double hurtboxSize, sf::RenderWindow& window, const std::vector<std::string>& textureFiles);

    void takeDamage(int damage);
    void handleActions();
    void draw();
    void moveCaptured();

    int getHp() const;
    std::vector<std::pair<int, int>>& getHurtboxPixels();

private:
    int hp;
    int maxHP;
    double speed;
    double hurtboxSize;
    sf::RenderWindow& window;
    bool moving = false;
    bool waiting = false;
    bool TypeOneDrawFrame = true;
    const int frameTypeTimerBaseline = 30;
    int frameTypeTimer = 10;
    int frameVariation = 10;
    const float winCircleVelocity = 10;
    bool captured = false;
    int flyCooldown = 30;
    int moveCooldown = 0;
    int waitCooldown = 0;
    int baselineWaitTime;
    int baselineMoveTime;
    int moveTimer = 0;
    int waitTimer = 0;
    int xSpeed = 0;
    int ySpeed = 0;

    // constant and not relevant to any non-constant variables
    const int numDirections = 8;
    const int healthbarOffset = 17;
    const int spriteScaleFactor = 2;
    const int circleScaleNum = 6;
    const int circleScaleDenom = 5;
    const float healthBarScaleFactor = 2.5;
    const float healthBarHeight = 5.0f;


    std::vector<std::pair<int, int>> hurtboxPixels;
    std::pair<int, int> position;
    sf::CircleShape winCircle;
    sf::Texture DownLeft1Texture; 
    sf::Texture DownLeft2Texture;
    sf::Texture DownRight1Texture;
    sf::Texture DownRight2Texture;
    sf::Texture UpLeft1Texture;
    sf::Texture UpLeft2Texture;
    sf::Texture UpRight1Texture;
    sf::Texture UpRight2Texture;
    sf::Sprite monsterSprite;
    /*It is important that the textures are stored as member variables since setting a texture to a sprite only stores a pointer to the texture instance
    If a texture is stored as a member variable here, it can safely be passed between member functions without the texture being lost*/

    void loadTextures(const std::vector<std::string>& textureFiles);
    void initializeHurtboxPixels();
    void move();
    void decideDirection();
    void startMoving();
    void startWaiting();
    sf::FloatRect calculateBounds() const;
    void drawHealthBar(const sf::FloatRect& bounds) const;
    sf::Sprite createSprite(const sf::FloatRect& bounds);
};
