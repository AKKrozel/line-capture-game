#define _USE_MATH_DEFINES

#include "Monster.h"
#include <cstdlib>
#include <cmath>
#include <iostream>

Monster::Monster(int hp, double speed, int baselineWaitTime, int baselineMoveTime, std::pair<int, int> position, double hurtboxSize, sf::RenderWindow& window, const std::vector<std::string>& textureFiles)
    : hp(hp), maxHP(hp), speed(speed), baselineWaitTime(baselineWaitTime), baselineMoveTime(baselineMoveTime), position(position), hurtboxSize(hurtboxSize), window(window) {

    loadTextures(textureFiles);
    initializeHurtboxPixels();
}

void Monster::loadTextures(const std::vector<std::string>& textureFiles) {
    DownLeft1Texture.loadFromFile(textureFiles[0]);
    DownLeft2Texture.loadFromFile(textureFiles[1]);
    DownRight1Texture.loadFromFile(textureFiles[2]);
    DownRight2Texture.loadFromFile(textureFiles[3]);
    UpLeft1Texture.loadFromFile(textureFiles[4]);
    UpLeft2Texture.loadFromFile(textureFiles[5]);
    UpRight1Texture.loadFromFile(textureFiles[6]);
    UpRight2Texture.loadFromFile(textureFiles[7]);
}

void Monster::initializeHurtboxPixels() {
    int centerX = position.first;
    int centerY = position.second;

    for (int x = centerX - hurtboxSize; x <= centerX + hurtboxSize; ++x) {
        for (int y = centerY - hurtboxSize; y <= centerY + hurtboxSize; ++y) {
            hurtboxPixels.push_back({ x, y });
        }
    }
}

void Monster::takeDamage(int damage) {
    hp -= damage;
    if (hp < 0) {
        hp = 0;
    }
}

void Monster::move() {
    for (auto& pixel : hurtboxPixels) {
        // Check if the pixel is getting close to the screen boundaries and set speed away from bouondary

        double borderBuffer = 3 * (hurtboxSize + speed);
        if (pixel.first < borderBuffer) {
            xSpeed = std::abs(xSpeed);
            break;
        }
        else if (pixel.first >= window.getSize().x - borderBuffer){
            xSpeed = -std::abs(xSpeed);
            break;
        }
        else if (pixel.second < borderBuffer) {
            ySpeed = std::abs(ySpeed);
            break;
        }
        else if (pixel.second >= window.getSize().y - borderBuffer) {
            ySpeed = -std::abs(ySpeed);
            break;
        }
    }
    for (auto& pixel : hurtboxPixels) {
        pixel.first += xSpeed;
        pixel.second += ySpeed;
    }
}

// A good value for decide directions is 4*n for integer n due to the symmetry of directions possible
void Monster::decideDirection() {
    int random = std::rand() % numDirections;
    xSpeed = static_cast<int>(speed * std::cos(random * M_PI * 2 / numDirections + M_PI / numDirections));
    ySpeed = static_cast<int>(speed * std::sin(random * M_PI * 2 / numDirections + M_PI / numDirections));
}

void Monster::handleActions() {
    if (waiting) {
        if (--waitTimer <= 0) {
            waiting = false;
        }
    }
    else if (moving) {
        move();
        if (--moveTimer <= 0) {
            moving = false;
        }
    }
    else {
        if (moveCooldown > 0) {
            startWaiting();
        }
        else if (waitCooldown > 0) {
            startMoving();
        }
        else {
            int randomDecision = std::rand() % 2;
            if (randomDecision == 0) {
                startWaiting();
            }
            else {
                startMoving();
            }
        }
    }
    --moveCooldown;
    --waitCooldown;
}

void Monster::startMoving() {
    moving = true;
    moveCooldown = 0;
    moveTimer = baselineMoveTime;
    decideDirection();
}

void Monster::startWaiting() {
    waiting = true;
    waitCooldown = 0;
    waitTimer = baselineWaitTime;
}

sf::FloatRect Monster::calculateBounds() const {
    int minX = hurtboxPixels[0].first;
    int minY = hurtboxPixels[0].second;
    return sf::FloatRect(static_cast<float>(minX), static_cast<float>(minY), static_cast<float>(2 * hurtboxSize + 1), static_cast<float>(2 * hurtboxSize + 1));
}

void Monster::draw() {
    sf::FloatRect bounds = calculateBounds();

    if (!captured) {
        drawHealthBar(bounds);
    }

    sf::Sprite sprite = createSprite(bounds);

    if (!captured) {
        position.first = bounds.left + bounds.width / 2;
        position.second = bounds.top + bounds.height / 2;
    }

    sprite.setPosition(position.first, position.second);
    window.draw(sprite);
}

void Monster::drawHealthBar(const sf::FloatRect& bounds) const {
    float healthBarWidth = healthBarScaleFactor * bounds.width;
    float greenHealthBarWidth = static_cast<float>(hp) / maxHP * healthBarWidth;

    sf::RectangleShape greenHealthBar(sf::Vector2f(greenHealthBarWidth, healthBarHeight));
    greenHealthBar.setFillColor(sf::Color::Green);
    greenHealthBar.setPosition(bounds.left - (healthBarScaleFactor - 1.0) * bounds.width / 2.0, bounds.top + bounds.height + healthbarOffset);

    sf::RectangleShape orangeBackground(sf::Vector2f(healthBarWidth, healthBarHeight));
    orangeBackground.setFillColor(sf::Color(255, 165, 0));
    orangeBackground.setPosition(bounds.left - (healthBarScaleFactor - 1.0) * bounds.width / 2.0, bounds.top + bounds.height + healthbarOffset);

    window.draw(orangeBackground);
    window.draw(greenHealthBar);
}

sf::Sprite Monster::createSprite(const sf::FloatRect& bounds) {
    sf::Sprite sprite;

    if (TypeOneDrawFrame) {
        if (ySpeed >= 0 && xSpeed <= 0) {
            sprite.setTexture(DownLeft1Texture);
        }
        else if (ySpeed >= 0 && xSpeed >= 0) {
            sprite.setTexture(DownRight1Texture);
        }
        else if (ySpeed <= 0 && xSpeed <= 0) {
            sprite.setTexture(UpLeft1Texture);
        }
        else {
            sprite.setTexture(UpRight1Texture);
        }
    }
    else {
        if (ySpeed >= 0 && xSpeed <= 0) {
            sprite.setTexture(DownLeft2Texture);
        }
        else if (ySpeed >= 0 && xSpeed >= 0) {
            sprite.setTexture(DownRight2Texture);
        }
        else if (ySpeed <= 0 && xSpeed <= 0) {
            sprite.setTexture(UpLeft2Texture);
        }
        else {
            sprite.setTexture(UpRight2Texture);
        }
    }

    frameTypeTimer -= 1;
    if (frameTypeTimer <= 0) {
        frameTypeTimer = frameTypeTimerBaseline + (std::rand() % frameVariation);
        TypeOneDrawFrame = !TypeOneDrawFrame;

    }

    sf::FloatRect spriteBounds = sprite.getLocalBounds();
    sprite.setOrigin(spriteBounds.width / 2, spriteBounds.height / 2);

    float scaleX = bounds.width / spriteBounds.width;
    float scaleY = bounds.height / spriteBounds.height;
    sprite.setScale(scaleX * spriteScaleFactor, scaleY * spriteScaleFactor);

    return sprite;
}


void Monster::moveCaptured() {
    captured = true;

    sf::FloatRect bounds = calculateBounds();
    float radius = std::max(bounds.width, bounds.height) * circleScaleNum/circleScaleDenom;

    winCircle.setRadius(radius);
    winCircle.setPosition(position.first - radius, position.second - radius);
    winCircle.setFillColor(sf::Color(255, 255, 0, 128));
    winCircle.setOutlineThickness(5.0f);
    winCircle.setOutlineColor(sf::Color(255, 255, 0, 200));

    if (flyCooldown <= 0) {
        position.second -= winCircleVelocity;
    }
    else {
        flyCooldown--;
        xSpeed = -1;
        ySpeed = 1;
    }

    window.draw(winCircle);
}

int Monster::getHp() const {
    return hp;
}


std::vector<std::pair<int, int>>& Monster::getHurtboxPixels() {
    return hurtboxPixels;
}
