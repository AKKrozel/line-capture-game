#include <SFML/Graphics.hpp>
#include "Monster.h"
#include "CaptureDevice.h"
#include <vector>

void playVictoryLoop(CaptureDevice& CD, std::vector<Monster>& monsters, sf::RenderWindow& window, sf::Sprite& background, sf::Font font, bool& restartGame);
void playLossLoop(CaptureDevice& CD, std::vector<Monster>& monsters, sf::RenderWindow& window, sf::Sprite& background, sf::Font font, bool& restartGame);
void playRegularLoop(CaptureDevice& CD, std::vector<Monster>& monsters, sf::RenderWindow& window, sf::Sprite& background, bool& won, bool& lost);

int main() {

    // window
    const double windowWidth = 850;
    const double windowHeight = 850;
    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "Game");
    window.setFramerateLimit(25);

    // background sprite
    sf::Texture backgroundTexture;
    backgroundTexture.loadFromFile("CoralBGSquare.png");
    sf::Sprite background(backgroundTexture);
    sf::Vector2u textureSize = backgroundTexture.getSize();
    sf::Vector2u windowSize = window.getSize();
    float scaleX = static_cast<float>(windowSize.x) / textureSize.x;
    float scaleY = static_cast<float>(windowSize.y) / textureSize.y;
    background.setScale(scaleX, scaleY);

    // text
    sf::Font font;
    font.loadFromFile("arial.ttf");

    // monsters
    int monsterHP = 50;
    double Speed = 6.0;
    int baselineTime = 20;
    double hurtboxSize = 15.0;
    std::vector<std::string> textureFiles = {
        "HydrormLF.png",
        "HydrormLFS.png",
        "HydrormRF.png",
        "HydrormRFS.png",
        "HydrormLB.png",
        "HydrormLBS.png",
        "HydrormRB.png",
        "HydrormRBS.png"
    };
    std::pair<int, int> position1 = { static_cast<int>(window.getSize().x * 0.5),static_cast<int>(window.getSize().y * 0.5) };
    std::pair<int, int> position2 = { static_cast<int>(window.getSize().x * 0.45),static_cast<int>(window.getSize().y * 0.45) };
    std::pair<int, int> position3 = { static_cast<int>(window.getSize().x * 0.55),static_cast<int>(window.getSize().y * 0.55) };
    std::vector<Monster> monsters;
    monsters.emplace_back(monsterHP, Speed, baselineTime, baselineTime, position1, hurtboxSize, window, textureFiles);
    monsters.emplace_back(monsterHP, Speed, baselineTime, baselineTime, position2, hurtboxSize, window, textureFiles);
    monsters.emplace_back(monsterHP, Speed, baselineTime, baselineTime, position3, hurtboxSize, window, textureFiles);

    // CaptureDevice
    double length = 1500;
    double CDHP = 100;
    CaptureDevice CD(CDHP, length, window);

    // Game loop
    bool lost = false;
    bool won = false;
    bool restartGame = false;

    // Game over loops
    while (window.isOpen()) {
        if (!lost && !won) {
            playRegularLoop(CD, monsters, window, background, won, lost);
        }
        if (won) {
            playVictoryLoop(CD, monsters, window, background, font, restartGame);
        }
        if (lost) {
            playLossLoop(CD, monsters, window, background, font, restartGame);
        }
        if (restartGame == true) {
            lost = false;
            won = false;
            monsters.clear();
            monsters.emplace_back(monsterHP, Speed, baselineTime, baselineTime, position1, hurtboxSize, window, textureFiles);
            monsters.emplace_back(monsterHP, Speed, baselineTime, baselineTime, position2, hurtboxSize, window, textureFiles);
            monsters.emplace_back(monsterHP, Speed, baselineTime, baselineTime, position3, hurtboxSize, window, textureFiles);
            CD.reset();
            restartGame = false;
        }
    }

    return 0;
}

// Game loop functions

void playVictoryLoop(CaptureDevice& CD, std::vector<Monster>& monsters, sf::RenderWindow& window, sf::Sprite& background, sf::Font font, bool& restartGame) {
    sf::Text successText("SUCCESS", font, 72);
    successText.setFillColor(sf::Color::Green);
    successText.setPosition(window.getSize().x * 0.5f - successText.getGlobalBounds().width / 2, window.getSize().y * 0.2f);

    while (window.isOpen() && !restartGame) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            else if (event.type == sf::Event::MouseButtonPressed) {
                restartGame = true; // Set restart flag on mouse click
            }
        }

        window.clear();
        window.draw(background);
        CD.draw();
        for (Monster& monster : monsters) {
            monster.draw();
            monster.moveCaptured();
        }
        window.draw(successText);

        window.display();
    }
}

void playLossLoop(CaptureDevice& CD, std::vector<Monster>& monsters, sf::RenderWindow& window, sf::Sprite& background, sf::Font font, bool& restartGame) {
    sf::Text failureText("FAILURE", font, 72);
    failureText.setFillColor(sf::Color::Red);
    failureText.setPosition(window.getSize().x * 0.5f - failureText.getGlobalBounds().width / 2, window.getSize().y * 0.2f);

    while (window.isOpen() && !restartGame) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            else if (event.type == sf::Event::MouseButtonPressed) {
                restartGame = true; // Set restart flag on mouse click
            }
        }

        window.clear();
        window.draw(background);
        CD.drawLoss();
        for (Monster& monster : monsters) {
            monster.handleActions();
            monster.draw();
        }
        window.draw(failureText);

        window.display();
    }
}

void playRegularLoop(CaptureDevice& CD, std::vector<Monster>& monsters, sf::RenderWindow& window, sf::Sprite& background, bool& won, bool& lost) {

    sf::Event event;
    while (window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
            window.close();
    }
    window.clear();
    window.draw(background);
    CD.update();
    int captureCount = 0;
    for (Monster& monster : monsters) {
        monster.draw();
        if (monster.getHp() > 0) {
            CD.considerMonsterCollision(monster, lost);
            monster.handleActions();
        }
        else {
            monster.moveCaptured();
            captureCount++;
        }

    }

    if (captureCount >= monsters.size()) {
        won = true;
        CD.informGameOver();
    }

    CD.considerSelfCollision(monsters);
    CD.draw();

    window.display();


}
