#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

enum class AppState {
    MainMenu,
    SimTopicsMenu,
    MotionOneDimSubMenu
};

std::vector<sf::Text> createMenuTextObj(const std::vector<std::string>& menuItems, const sf::Font& font, sf::Color textColor);
sf::RectangleShape positionMenuAndCreateBox(std::vector<sf::Text>& textObjects, sf::RenderWindow& window, sf::Color highlightColor);
