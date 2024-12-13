#include "ProjectileMotion.h"
#include "Ball.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>
#include <string>
#include <sstream>
#include <iomanip>

enum ActiveField { None, SpeedField, GravityField }; // Angle field is read-only

void runProjectileMotionSimulation() {
    sf::RenderWindow window(sf::VideoMode(1920, 1080), "Projectile Motion Simulator", sf::Style::Close);
    window.setFramerateLimit(60);

    sf::Font font;
    if (!font.loadFromFile("retrogaming.ttf")) {
        std::cerr << "Error: Could not load font 'retrogaming.ttf'.\n";
        return;
    }

    // Load textures
    sf::Texture background;
    if (!background.loadFromFile("src/MotionInDimensions/imgs/background.jpg")) {
        std::cerr << "Error: Could not load 'background.jpg'.\n";
        return;
    }

    sf::Texture character;
    if (!character.loadFromFile("src/MotionInDimensions/imgs/character.png")) {
        std::cerr << "Error: Could not load 'character.png'.\n";
        return;
    }

    sf::Texture ballTex;
    if (!ballTex.loadFromFile("src/MotionInDimensions/imgs/ball.png")) {
        std::cerr << "Error: Could not load 'ball.png'.\n";
        return;
    }

    sf::Texture cart;
    if (!cart.loadFromFile("src/MotionInDimensions/imgs/cart.png")) {
        std::cerr << "Error: Could not load 'cart.png'.\n";
        return;
    }

    sf::Sprite spriteBackground(background);
    sf::Vector2u textureSize = background.getSize();
    sf::Vector2u windowSize = window.getSize();
    float scaleX = static_cast<float>(windowSize.x) / textureSize.x;
    float scaleY = static_cast<float>(windowSize.y) / textureSize.y;
    spriteBackground.setScale(scaleX, scaleY);

    float groundLineY = 800.f; // baseline for basket and initial character

    sf::Sprite spriteCharacter(character);
    {
        sf::FloatRect charBounds = spriteCharacter.getLocalBounds();
        spriteCharacter.setOrigin(charBounds.width / 2.f, charBounds.height / 2.f);
        spriteCharacter.setPosition(118.f, groundLineY);
    }

    sf::Sprite spriteCart(cart);
    {
        sf::FloatRect cartBounds = spriteCart.getLocalBounds();
        spriteCart.setOrigin(cartBounds.width / 2.f, cartBounds.height / 2.f);
        spriteCart.setPosition(1550.f, groundLineY);
    }

    // Buttons
    sf::RectangleShape simulateButton(sf::Vector2f(200.f, 60.f));
    simulateButton.setFillColor(sf::Color::Blue);
    simulateButton.setPosition(860.f, 900.f);

    sf::Text buttonText("Simulate", font, 30);
    {
        sf::FloatRect btnBounds = buttonText.getLocalBounds();
        buttonText.setOrigin(btnBounds.width / 2.f, btnBounds.height / 2.f);
        buttonText.setPosition(simulateButton.getPosition().x + simulateButton.getSize().x / 2.f,
            simulateButton.getPosition().y + simulateButton.getSize().y / 2.f);
    }

    sf::RectangleShape resetButton(sf::Vector2f(200.f, 60.f));
    resetButton.setFillColor(sf::Color::Red);
    resetButton.setPosition(1080.f, 900.f);

    sf::Text resetText("Reset", font, 30);
    {
        sf::FloatRect rtBounds = resetText.getLocalBounds();
        resetText.setOrigin(rtBounds.width / 2.f, rtBounds.height / 2.f);
        resetText.setPosition(resetButton.getPosition().x + resetButton.getSize().x / 2.f,
            resetButton.getPosition().y + resetButton.getSize().y / 2.f);
    }

    float scale = 100.f;

    bool simulationRunning = false;
    bool goalScored = false;
    bool outOfBounds = false;
    bool ballInitialized = false;
    Ball volleyball(0, 0, 0, 0);

    sf::Text statusText;
    statusText.setFont(font);
    statusText.setCharacterSize(60);
    statusText.setFillColor(sf::Color::Yellow);
    statusText.setPosition(800.f, 500.f);

    bool draggingCharacter = false;
    bool draggingCart = false;
    bool draggingArrow = false;
    sf::Vector2f dragOffset;

    // Parameters
    std::string speedStr = "11.5";
    std::string gravityStr = "9.8";
    float arrowAngle = 45.0f; // degrees (0°=Up, 90°=Right)
    std::string angleStr = "45.0";

    // Text fields
    sf::RectangleShape speedFieldRect(sf::Vector2f(150.f, 40.f));
    speedFieldRect.setFillColor(sf::Color::White);
    speedFieldRect.setPosition(windowSize.x - 200.f, 50.f);

    sf::RectangleShape angleFieldRect(sf::Vector2f(150.f, 40.f));
    angleFieldRect.setFillColor(sf::Color::White);
    angleFieldRect.setPosition(windowSize.x - 200.f, 120.f);

    sf::RectangleShape gravityFieldRect(sf::Vector2f(150.f, 40.f));
    gravityFieldRect.setFillColor(sf::Color::White);
    gravityFieldRect.setPosition(windowSize.x - 200.f, 190.f);

    sf::Text speedLabel("Speed (m/s):", font, 20);
    speedLabel.setFillColor(sf::Color::Black);
    speedLabel.setPosition(windowSize.x - 340.f, 50.f);

    sf::Text angleLabel("Angle (deg):", font, 20);
    angleLabel.setFillColor(sf::Color::Black);
    angleLabel.setPosition(windowSize.x - 340.f, 120.f);

    sf::Text gravityLabel("Gravity (m/s^2):", font, 20);
    gravityLabel.setFillColor(sf::Color::Black);
    gravityLabel.setPosition(windowSize.x - 380.f, 190.f);

    sf::Text speedText(speedStr, font, 20);
    speedText.setFillColor(sf::Color::Black);
    speedText.setPosition(speedFieldRect.getPosition().x + 10.f, speedFieldRect.getPosition().y + 5.f);

    sf::Text angleText(angleStr, font, 20); // read-only angle
    angleText.setFillColor(sf::Color::Black);
    angleText.setPosition(angleFieldRect.getPosition().x + 10.f, angleFieldRect.getPosition().y + 5.f);

    sf::Text gravityText(gravityStr, font, 20);
    gravityText.setFillColor(sf::Color::Black);
    gravityText.setPosition(gravityFieldRect.getPosition().x + 10.f, gravityFieldRect.getPosition().y + 5.f);

    ActiveField activeField = None;

    // Distance and Height display
    sf::Text distanceText("", font, 30);
    distanceText.setFillColor(sf::Color::Yellow);
    distanceText.setPosition(100.f, 100.f);

    sf::Text heightText("", font, 30);
    heightText.setFillColor(sf::Color::Yellow);
    heightText.setPosition(100.f, 150.f);

    // Arrow offset from character
    sf::Vector2f characterArrowOffset(30.f, -175.f);

    // Arrow shape
    sf::ConvexShape arrowShape;
    arrowShape.setPointCount(7);
    arrowShape.setPoint(0, sf::Vector2f(-2.5f, 0.f));      // bottom left
    arrowShape.setPoint(1, sf::Vector2f(2.5f, 0.f));       // bottom right
    arrowShape.setPoint(2, sf::Vector2f(2.5f, -100.f));    // shaft top right
    arrowShape.setPoint(3, sf::Vector2f(10.f, -100.f));    // tip right base
    arrowShape.setPoint(4, sf::Vector2f(0.f, -120.f));     // tip top
    arrowShape.setPoint(5, sf::Vector2f(-10.f, -100.f));   // tip left base
    arrowShape.setPoint(6, sf::Vector2f(-2.5f, -100.f));   // shaft top left
    arrowShape.setFillColor(sf::Color::Red);

    // Platform under character
    sf::RectangleShape platformRect;
    platformRect.setFillColor(sf::Color(139, 69, 19));
    float platformWidth = 160.f;

    auto parseFloat = [](const std::string& str, float defaultVal)->float {
        try {
            return std::stof(str);
        }
        catch (...) {
            return defaultVal;
        }
        };

    auto resetSimulation = [&]() {
        simulationRunning = false;
        goalScored = false;
        outOfBounds = false;
        ballInitialized = false;

        // Reset positions
        spriteCharacter.setPosition(118.f, groundLineY);
        spriteCart.setPosition(1550.f, groundLineY);

        // Reset fields
        speedStr = "11.5";
        gravityStr = "9.8";
        speedText.setString(speedStr);
        gravityText.setString(gravityStr);

        // Reset angle
        arrowAngle = 45.0f;
        angleStr = "45.0";
        angleText.setString(angleStr);

        activeField = None;
        };

    while (window.isOpen()) {
        float dt = 1.f / 60.f;

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            else if (event.type == sf::Event::Resized) {
                sf::Vector2u newWindowSize(event.size.width, event.size.height);
                float scaleX = static_cast<float>(newWindowSize.x) / textureSize.x;
                float scaleY = static_cast<float>(newWindowSize.y) / textureSize.y;
                spriteBackground.setScale(scaleX, scaleY);
            }
            else if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2f mousePos = (sf::Vector2f)sf::Mouse::getPosition(window);
                if (!simulationRunning && !ballInitialized) {
                    if (simulateButton.getGlobalBounds().contains(mousePos)) {
                        float initialSpeed = parseFloat(speedStr, 11.5f);
                        float gravityVal = parseFloat(gravityStr, 9.8f);

                        // Convert arrowAngle to projectile angle
                        // arrowAngle: 0°=Up, 90°=Right, 180°=Down, 270°=Left
                        // projectile angle: 0°=Right, 90°=Up, etc.
                        float initialAngle = 90.f - arrowAngle;

                        simulationRunning = true;

                        float ballStartX_m = spriteCharacter.getPosition().x / scale;
                        float ballStartY_m = (spriteCharacter.getPosition().y - 251.f) / scale;
                        volleyball = Ball(ballStartX_m, ballStartY_m, initialSpeed, initialAngle, gravityVal, scale);

                        sf::Sprite spriteBall(ballTex);
                        spriteBall.setScale(0.25f, 0.25f);
                        sf::FloatRect ballBounds = spriteBall.getLocalBounds();
                        spriteBall.setOrigin(ballBounds.width / 2.f, ballBounds.height / 2.f);
                        volleyball.setSprite(spriteBall);

                        ballInitialized = true;
                    }
                    else {
                        // Check character (vertical only)
                        if (spriteCharacter.getGlobalBounds().contains(mousePos)) {
                            draggingCharacter = true;
                            dragOffset = spriteCharacter.getPosition() - mousePos;
                        }
                        // Check cart (horizontal only)
                        else if (spriteCart.getGlobalBounds().contains(mousePos)) {
                            draggingCart = true;
                            dragOffset = spriteCart.getPosition() - mousePos;
                        }
                        // Check speed field
                        else if (speedFieldRect.getGlobalBounds().contains(mousePos)) {
                            activeField = SpeedField;
                        }
                        // Check gravity field
                        else if (gravityFieldRect.getGlobalBounds().contains(mousePos)) {
                            activeField = GravityField;
                        }
                        else {
                            // Check arrow
                            sf::FloatRect arrowBounds = arrowShape.getGlobalBounds();
                            if (arrowBounds.contains(mousePos)) {
                                draggingArrow = true;
                            }
                            else {
                                activeField = None;
                            }
                        }
                    }
                }
                else if (!simulationRunning && ballInitialized) {
                    // Simulation ended, check reset button
                    if (resetButton.getGlobalBounds().contains(mousePos)) {
                        resetSimulation();
                    }
                }
            }
            else if (event.type == sf::Event::MouseButtonReleased) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    draggingCharacter = false;
                    draggingCart = false;
                    draggingArrow = false;
                }
            }
            else if (event.type == sf::Event::MouseMoved && !simulationRunning && !ballInitialized) {
                sf::Vector2f mousePos = (sf::Vector2f)sf::Mouse::getPosition(window);

                if (draggingCharacter) {
                    // Move character vertically only
                    float newY = mousePos.y + dragOffset.y;
                    spriteCharacter.setPosition(spriteCharacter.getPosition().x, newY);
                }
                else if (draggingCart) {
                    // Move cart horizontally only
                    float newX = mousePos.x + dragOffset.x;
                    spriteCart.setPosition(newX, spriteCart.getPosition().y);
                }
                else if (draggingArrow) {
                    // Compute angle:
                    // 0°=Up, 90°=Right
                    sf::Vector2f arrowPivot = spriteCharacter.getPosition() + characterArrowOffset;
                    sf::Vector2f diff = mousePos - arrowPivot;
                    float angleRad = std::atan2(diff.y, diff.x);
                    float angleDeg = angleRad * 180.f / 3.14159f + 90.f;
                    arrowAngle = angleDeg;

                    std::stringstream ss;
                    ss << std::fixed << std::setprecision(1) << arrowAngle;
                    angleStr = ss.str();
                    angleText.setString(angleStr);
                }
            }
            else if (event.type == sf::Event::TextEntered && !simulationRunning && !ballInitialized) {
                if (activeField != None) {
                    uint32_t unicode = event.text.unicode;
                    std::string* target = nullptr;
                    sf::Text* targetText = nullptr;

                    if (activeField == SpeedField) { target = &speedStr; targetText = &speedText; }
                    else if (activeField == GravityField) { target = &gravityStr; targetText = &gravityText; }

                    if (target) {
                        if (unicode == '\r') {
                            // Enter pressed - stop editing
                            activeField = None;
                        }
                        else if (unicode == '\b') {
                            // Backspace
                            if (!target->empty()) {
                                target->pop_back();
                                targetText->setString(*target);
                            }
                        }
                        else if ((unicode >= '0' && unicode <= '9') || unicode == '.') {
                            // Only one decimal point allowed
                            if (unicode == '.' && target->find('.') != std::string::npos) {
                                // Ignore second decimal point
                            }
                            else {
                                target->push_back((char)unicode);
                                targetText->setString(*target);
                            }
                        }
                    }
                }
            }
        }

        // Update arrow position each frame
        sf::Vector2f arrowPivot = spriteCharacter.getPosition() + characterArrowOffset;
        arrowShape.setPosition(arrowPivot);
        arrowShape.setRotation(arrowAngle);

        // Update distance and height
        float dist = std::fabs(spriteCharacter.getPosition().x - spriteCart.getPosition().x) / scale;
        {
            std::stringstream ss;
            ss << "Distance: " << std::fixed << std::setprecision(2) << dist << " m";
            distanceText.setString(ss.str());
        }

        float heightDiff = (spriteCharacter.getPosition().y - spriteCart.getPosition().y) / scale;
        heightDiff = std::fabs(heightDiff);
        {
            std::stringstream ss;
            ss << "Height: " << std::fixed << std::setprecision(2) << heightDiff << " m";
            heightText.setString(ss.str());
        }

        // Update platform
        float charY = spriteCharacter.getPosition().y;
        float charX = spriteCharacter.getPosition().x;
        if (charY < groundLineY) {
            float platformHeight = groundLineY - charY;
            platformRect.setSize(sf::Vector2f(platformWidth, platformHeight));
            platformRect.setPosition(charX - platformWidth / 2.f, charY + 130.f);
        }
        else {
            platformRect.setSize(sf::Vector2f(0.f, 0.f));
        }

        // Physics update
        if (simulationRunning && ballInitialized) {
            volleyball.update(dt);
            float basketX_m = spriteCart.getPosition().x / scale;
            float basketY_m = spriteCart.getPosition().y / scale;

            if (volleyball.isScored(basketX_m, basketY_m, 1.0f)) {
                simulationRunning = false;
                goalScored = true;
            }

            if (volleyball.isOutOfBounds(static_cast<float>(windowSize.x) / scale,
                static_cast<float>(windowSize.y) / scale)) {
                simulationRunning = false;
                outOfBounds = true;
            }
        }

        // Rendering
        window.clear();
        window.draw(spriteBackground);

        // Draw platform if any
        if (platformRect.getSize().y > 0.f) {
            window.draw(platformRect);
        }

        window.draw(spriteCharacter);
        window.draw(spriteCart);
        window.draw(distanceText);
        window.draw(heightText);

        if (!ballInitialized && !simulationRunning) {
            // Draw arrow
            window.draw(arrowShape);

            // Draw UI fields and Simulate button
            window.draw(speedFieldRect);
            window.draw(angleFieldRect);
            window.draw(gravityFieldRect);

            window.draw(speedLabel);
            window.draw(angleLabel);
            window.draw(gravityLabel);

            // Editing cursor in fields
            if (activeField == SpeedField) {
                sf::FloatRect speedBounds = speedText.getLocalBounds();
                sf::Text cursor("|", font, 20);
                cursor.setFillColor(sf::Color::Black);
                cursor.setPosition(speedText.getPosition().x + speedBounds.width + 2.f, speedText.getPosition().y);
                window.draw(speedText);
                window.draw(cursor);
            }
            else {
                window.draw(speedText);
            }

            window.draw(angleText);

            if (activeField == GravityField) {
                sf::FloatRect gravBounds = gravityText.getLocalBounds();
                sf::Text cursor("|", font, 20);
                cursor.setFillColor(sf::Color::Black);
                cursor.setPosition(gravityText.getPosition().x + gravBounds.width + 2.f, gravityText.getPosition().y);
                window.draw(gravityText);
                window.draw(cursor);
            }
            else {
                window.draw(gravityText);
            }

            window.draw(simulateButton);
            window.draw(buttonText);
        }

        if (ballInitialized) {
            // Draw ball
            volleyball.draw(window);
        }

        if (!simulationRunning && ballInitialized) {
            // Simulation ended, show result and reset
            if (goalScored) {
                statusText.setString("Goal!");
            }
            else if (outOfBounds) {
                statusText.setString("No Goal!");
            }
            window.draw(statusText);

            window.draw(resetButton);
            window.draw(resetText);
        }

        window.display();
    }
}
