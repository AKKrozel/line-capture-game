#include "CaptureDevice.h"
#include "Monster.h"

// Custom hash function
namespace std {
    template <>
    struct hash<std::pair<int, int>> {
        std::size_t operator()(const std::pair<int, int>& p) const {
            auto h1 = std::hash<int>{}(p.first);
            auto h2 = std::hash<int>{}(p.second);
            return h1 ^ (h2 << 1);
        }
    };
}


CaptureDevice::CaptureDevice(double hp, int length, sf::RenderWindow& window)
    : length(length), window(window), hp(hp) {
    maxHP = hp;
}

void CaptureDevice::update() {
    auto mousePos = sf::Mouse::getPosition(window);
    if (mousePos.x >= 0 && mousePos.x < window.getSize().x &&
        mousePos.y >= 0 && mousePos.y < window.getSize().y) {

        std::pair<int, int> currentPixel = { mousePos.x, mousePos.y };

        if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            if (justTookDamage) {
                return;
            }

            if (!heldPixels.empty()) {
                auto lastPixel = heldPixels.back();
                addLineOfPixels(lastPixel, currentPixel);
            }

            if (heldPixels.empty()) {
                heldPixels.push_back(currentPixel);
            }


            while (heldPixels.size() > static_cast<size_t>(length)) {
                auto front = heldPixels.front();
                heldPixels.pop_front();
            }
            
        }
        else {
            justTookDamage = false;
            heldPixels.clear();
        }
    }
}

// Check the pixel to add against the last n pixels added to make sure it isn't a duplicate, this helps with handling sharp cut backs on the line drawn
void CaptureDevice::addIfAppropriate(int x1, int y1) {
    bool isDuplicate = false;
    int numPixelsToCheck = std::min(pixelsToCheckBeforeAdd, static_cast<int>(heldPixels.size()));
    for (int i = 1; i <= numPixelsToCheck; ++i) {
        if (heldPixels[heldPixels.size() - i] == std::make_pair(x1, y1)) {
            isDuplicate = true;
            break;
        }
    }

    if (!isDuplicate) {
        heldPixels.push_back({ x1, y1 });
    }
}

/*This is an Altered Bresenham's line algorithm that adds an extra pixel whenever there are two tiles only connected by their corners. 
The alteration forces a pixel overlap when the drawn lines cross as the pixels composing the lines will always be adjacent side-to-side, never just corner-to-corner
The alteration comes at the cost of a less pretty line and slightly higher computation time but is certainly worthwhile*/
void CaptureDevice::addLineOfPixels(std::pair<int, int> start, std::pair<int, int> end) {
    int x1 = start.first;
    int y1 = start.second;
    int x2 = end.first;
    int y2 = end.second;

    int dx = std::abs(x2 - x1);
    int dy = std::abs(y2 - y1);

    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;

    int err = dx - dy;

    while (x1 != x2 || y1 != y2) {
        int err2 = 2 * err;
        if (err2 > -dy) {
            err -= dy;
            x1 += sx;
        }
        else if (err2 < dx) {
            err += dx;
            y1 += sy;
        }
        addIfAppropriate(x1, y1);
        if (err2 > -dy && err2 < dx) {
            err += dx;
            y1 += sy;
        }
        addIfAppropriate(x1, y1);
    }
}


void CaptureDevice::considerMonsterCollision(Monster& monster, bool& lost) {
    for (const auto& pixel : heldPixels) {
        for (const auto& monsterPixel : monster.getHurtboxPixels()) {
            if (pixel == monsterPixel) {
                takeDamage(damageTakenByCD);
                heldPixels.clear();
                justTookDamage = true;
                if (hp == 0) {
                    lost = true;
                }
                return;
            }
        }
    }
}



/*Check how many times a straight line starting at the monster crosses through the loop that will be removed
If there is an odd number of crossings, the monster is enclosed*/
bool CaptureDevice::lineCheck(const std::pair<int, int>& start, const std::unordered_set<std::pair<int, int>>& pixelSet, Direction direction) {
    int x = start.first;
    int y = start.second;
    bool seemsEnclosed = false;
    bool lastEncounterInSet = false;

    while (x >= 0 && x < window.getSize().x && y >= 0 && y < window.getSize().y) {
        if (pixelSet.find({ x, y }) != pixelSet.end()) {
            if (!lastEncounterInSet) {
                lastEncounterInSet = true;
                seemsEnclosed = !seemsEnclosed;
            }
        }
        else {
            lastEncounterInSet = false;
        }
        switch (direction) {
        case Direction::UP: y--; break;
        case Direction::DOWN: y++; break;
        case Direction::LEFT: x--; break;
        case Direction::RIGHT: x++; break;
        }
    }
    return seemsEnclosed;
}


std::unordered_set<std::pair<int, int>> convertToSet(const std::deque<std::pair<int, int>>& deque) {
    return std::unordered_set<std::pair<int, int>>(deque.begin(), deque.end());
}


void CaptureDevice::considerSelfCollision(std::vector<Monster>& monsters) {
    std::unordered_set<std::pair<int, int>> uniquePixels(heldPixels.begin(), heldPixels.end());
    if (uniquePixels.size() != heldPixels.size()) {
        std::vector<std::pair<int, int>> overlappingPixels;

        for (auto it = heldPixels.begin(); it != heldPixels.end(); ++it) {
            if (std::count(heldPixels.begin(), heldPixels.end(), *it) > 1) {
                overlappingPixels.push_back(*it);
            }
        }

        if (!overlappingPixels.empty()) {
            // Create a set of the pixels that will be removed due to self-collision representing the loop drawn
            auto startIter = std::find(heldPixels.begin(), heldPixels.end(), overlappingPixels.front());
            std::deque<std::pair<int, int>> pixelsToRemove(startIter, heldPixels.end());
            std::unordered_set<std::pair<int, int>> pixelsToRemoveSet = convertToSet(pixelsToRemove);

            for (Monster& monster : monsters)
            {
                bool enclosed = false;
                std::vector<std::pair<int, int>> hurtboxPixels = monster.getHurtboxPixels();
                if (!hurtboxPixels.empty()) {
                    std::pair<int, int> firstPixel = hurtboxPixels[0];
                    // 3 line checks are needed to prevent false enclosement detection. 2 should almost always be enough but 1 line check can be easily abused
                    if (lineCheck(firstPixel, pixelsToRemoveSet, Direction::UP) &&
                        lineCheck(firstPixel, pixelsToRemoveSet, Direction::DOWN) &&
                        lineCheck(firstPixel, pixelsToRemoveSet, Direction::LEFT)) {
                        enclosed = true;
                    }
                }

                if (enclosed) {
                    monster.takeDamage(damageTakenByMonster);
                }
            }
            
            // Remove pixels after the first duplicate only after dealing with all monsters
            for (const auto& pixel : overlappingPixels) {
                auto duplicateIter = std::find(std::next(heldPixels.begin(),
                    std::distance(heldPixels.begin(),
                        std::find(heldPixels.begin(), heldPixels.end(), pixel))),
                    heldPixels.end(), pixel);
                heldPixels.erase(duplicateIter, heldPixels.end());
            }
            for (int k = 0; k < 10 && !heldPixels.empty(); k++) {
                auto front = heldPixels.front();
                heldPixels.pop_front();
            }
        }
    }
}

void CaptureDevice::takeDamage(int damage) {
    hp -= damage;
    if (hp <= 0) {
        hp = 0;
        gameOver = true;
    }
}

void CaptureDevice::draw() {
    
    sf::RectangleShape pixelShape(sf::Vector2f(1, 1));
    pixelShape.setFillColor(sf::Color(220,220,255));

    if (!gameOver) {
        for (const auto& pixel : heldPixels) {
            // Draw a 3x3 grid of pixels around the current pixel
            for (int i = -1; i <= 1; ++i) {
                for (int j = -1; j <= 1; ++j) {
                    pixelShape.setPosition(static_cast<float>(pixel.first + i), static_cast<float>(pixel.second + j));
                    window.draw(pixelShape);
                }
            }
        }
    }
    
    float maxHealthBarWidth = window.getSize().x / screenToHealthConst;
    float healthBarWidth = static_cast<float>(hp) / maxHP * maxHealthBarWidth;
    CaptureDevice::drawHealthRectangle(sf::Color::Red, maxHealthBarWidth, maxHealthBarWidth);
    CaptureDevice::drawHealthRectangle(sf::Color::Green, healthBarWidth, maxHealthBarWidth);

}

void CaptureDevice::drawLoss() {
    float maxHealthBarWidth = window.getSize().x / screenToHealthConst;
    drawHealthRectangle(sf::Color::Red, maxHealthBarWidth, maxHealthBarWidth);

}

void CaptureDevice::drawHealthRectangle(sf::Color color, float shorterLength, float longerLength) {
    float healthBarWidth = static_cast<float>(hp) / maxHP * longerLength;
    sf::RectangleShape rectangle(sf::Vector2f(shorterLength, healthBarHeight));
    rectangle.setFillColor(color);
    rectangle.setPosition(healthBarCornerOffset, healthBarCornerOffset);
    window.draw(rectangle);
}

void CaptureDevice::informGameOver() {
    gameOver = true;
}