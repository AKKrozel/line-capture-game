#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <deque>
#include <unordered_set>
#include <cmath>
#include <algorithm>
#include <queue>
#include <unordered_map>
#include <utility>
#include <cmath>
#include <iostream>
#include <iterator>

class Monster;

enum class Direction {
    UP,
    DOWN,
    LEFT,
    RIGHT,
};

class CaptureDevice {
public:
    CaptureDevice(double CDHP, int length, sf::RenderWindow& window);

    void reset();
    void update();
    void considerMonsterCollision(Monster& monster, bool& lost);
    void considerSelfCollision(std::vector<Monster>& monster);
    void takeDamage(int damage);
    void draw();
    void drawLoss();
    void informGameOver();




private:
    void addIfAppropriate(int x1, int y1);
    void addLineOfPixels(std::pair<int, int> start, std::pair<int, int> end);
    bool lineCheck(const std::pair<int, int>& start, const std::unordered_set<std::pair<int, int>>& heldPixelsSet, Direction direction);
    bool isDuplicatePixel(int x, int y, int numPixelsToCheck);
    void drawHealthRectangle(sf::Color color, float longerLength, float shorterLength);
    int length;
    int hp;
    int maxHP;
    bool justTookDamage = false;
    bool gameOver = false;
    sf::RenderWindow& window;
    std::deque<std::pair<int, int>> heldPixels;

    // constants
    const int pixelsToCheckBeforeAdd = 10;
    const int healthBarCornerOffset = 5;
    const int damageTakenByCD = 20;
    const int damageTakenByMonster = 10;
    const int screenToHealthConst = 5;
    const int healthBarHeight = 20;

};
