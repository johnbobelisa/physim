#include "Main.h"

std::vector<sf::Text> createMenuTextObj(const std::vector<std::string>& menuItems, const sf::Font& font, sf::Color textColor) {
    std::vector<sf::Text> textObjects;
    for (const auto& item : menuItems) {
        sf::Text text;
        text.setFont(font);
        text.setString(item);
        text.setFillColor(textColor);
        text.setCharacterSize(40);
        textObjects.push_back(text);
    }
    return textObjects;
}

sf::RectangleShape positionMenuAndCreateBox(std::vector<sf::Text>& textObjects, sf::RenderWindow& window, sf::Color highlightColor) {
    float maxWidth = 0.0f;
    for (auto& t : textObjects) {
        float width = t.getLocalBounds().width;
        if (width > maxWidth) maxWidth = width;
    }

    float lineSpacing = 50.0f;
    float totalHeight = textObjects.size() * lineSpacing;
    float centerX = window.getSize().x / 2.0f;
    float centerY = window.getSize().y / 2.0f;

    float startY = centerY - (totalHeight / 2.0f);
    for (size_t i = 0; i < textObjects.size(); ++i) {
        sf::FloatRect bounds = textObjects[i].getLocalBounds();
        float textWidth = bounds.width;
        float textHeight = bounds.height;
        textObjects[i].setPosition(
            centerX - (textWidth / 2.0f) - bounds.left,
            (startY + i * lineSpacing) - (textHeight / 2.0f) - bounds.top
        );
    }

    float paddingX = 50.0f;
    float paddingY = 50.0f;

    float minX = 999999.f, minY = 999999.f;
    float maxX = -999999.f, maxY = -999999.f;

    for (auto& t : textObjects) {
        sf::FloatRect globalBounds = t.getGlobalBounds();
        if (globalBounds.left < minX) minX = globalBounds.left;
        if (globalBounds.top < minY)  minY = globalBounds.top;
        float right = globalBounds.left + globalBounds.width;
        float bottom = globalBounds.top + globalBounds.height;
        if (right > maxX) maxX = right;
        if (bottom > maxY) maxY = bottom;
    }

    float menuWidth = (maxX - minX) + paddingX * 2.0f;
    float menuHeight = (maxY - minY) + paddingY * 2.0f;

    sf::RectangleShape menuBox(sf::Vector2f(menuWidth, menuHeight));
    menuBox.setPosition(minX - paddingX, minY - paddingY);
    menuBox.setFillColor(sf::Color::Transparent);
    menuBox.setOutlineColor(highlightColor);
    menuBox.setOutlineThickness(2.0f);

    return menuBox;
}
