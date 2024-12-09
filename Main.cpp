#include "Main.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <iostream>

int main() {
    sf::RenderWindow window(sf::VideoMode(1920, 1080), "Physics Simulations Dashboard", sf::Style::Close);
    window.setFramerateLimit(60);

    sf::Font font;
    if (!font.loadFromFile("retrogaming.ttf")) {
        return -1;
    }

    // Colors
    sf::Color bgColor = sf::Color::Black;
    sf::Color textColor = sf::Color::White;
    sf::Color highlightColor = sf::Color::Green;

    // Menus
    std::vector<std::string> mainMenuItems = { "Simulate", "Quit" };
    std::vector<std::string> simTopicsMenuItems = { "Motion in One Dimension" };
    std::vector<std::string> motionOneDimSubMenuItems = {
        "Displacement, Velocity, and Speed",
        "Instantaneous Velocity and Speed",
        "Acceleration",
        "Motion Diagrams",
        "One-Dimensional Motion with Constant Acceleration",
        "Freely Falling Objects",
        "Kinematic Equations Derived from Calculus"
    };

    AppState currentState = AppState::MainMenu;
    std::vector<std::string>* currentMenuItems = &mainMenuItems;
    std::vector<sf::Text> currentTextObjects = createMenuTextObj(*currentMenuItems, font, textColor);
    sf::RectangleShape currentMenuBox = positionMenuAndCreateBox(currentTextObjects, window, highlightColor);

    int selectedItem = 0;
    currentTextObjects[selectedItem].setFillColor(highlightColor);

    auto updateMenu = [&](std::vector<std::string>& items) {
        currentTextObjects = createMenuTextObj(items, font, textColor);
        currentMenuBox = positionMenuAndCreateBox(currentTextObjects, window, highlightColor);
        selectedItem = 0;
        currentTextObjects[selectedItem].setFillColor(highlightColor);
        };

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Up) {
                    currentTextObjects[selectedItem].setFillColor(textColor);
                    selectedItem = (selectedItem - 1 + (int)currentMenuItems->size()) % (int)currentMenuItems->size();
                    currentTextObjects[selectedItem].setFillColor(highlightColor);
                }
                else if (event.key.code == sf::Keyboard::Down) {
                    currentTextObjects[selectedItem].setFillColor(textColor);
                    selectedItem = (selectedItem + 1) % (int)currentMenuItems->size();
                    currentTextObjects[selectedItem].setFillColor(highlightColor);
                }
                else if (event.key.code == sf::Keyboard::Enter) {
                    std::string chosen = (*currentMenuItems)[selectedItem];

                    if (currentState == AppState::MainMenu) {
                        if (chosen == "Simulate") {
                            currentState = AppState::SimTopicsMenu;
                            currentMenuItems = &simTopicsMenuItems;
                            updateMenu(*currentMenuItems);
                        }
                        else if (chosen == "Quit") {
                            window.close();
                        }
                    }
                    else if (currentState == AppState::SimTopicsMenu) {
                        if (chosen == "Motion in One Dimension") {
                            currentState = AppState::MotionOneDimSubMenu;
                            currentMenuItems = &motionOneDimSubMenuItems;
                            updateMenu(*currentMenuItems);
                        }
                    }
                    else if (currentState == AppState::MotionOneDimSubMenu) {
                        // Here, you would start the simulation related to the chosen sub-topic.
                        // For now, let's just close to demonstrate.
                        window.close();
                    }
                }
                else if (event.key.code == sf::Keyboard::Escape) {
                    // Go back up one level in the menu hierarchy
                    if (currentState == AppState::MotionOneDimSubMenu) {
                        currentState = AppState::SimTopicsMenu;
                        currentMenuItems = &simTopicsMenuItems;
                        updateMenu(*currentMenuItems);
                    }
                    else if (currentState == AppState::SimTopicsMenu) {
                        currentState = AppState::MainMenu;
                        currentMenuItems = &mainMenuItems;
                        updateMenu(*currentMenuItems);
                    }
                    else if (currentState == AppState::MainMenu) {
                        window.close();
                    }
                }
            }
        }

        window.clear(bgColor);
        window.draw(currentMenuBox);
        for (auto& t : currentTextObjects) {
            window.draw(t);
        }
        window.display();
    }

    return 0;
}
