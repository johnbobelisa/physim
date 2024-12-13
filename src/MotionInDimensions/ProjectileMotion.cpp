#include "ProjectileMotion.h"
#include "Ball.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>
#include <string>
#include <sstream>

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
    sf::Sprite spriteCharacter(character);
    sf::Sprite spriteBall(ballTex);
    sf::Sprite spriteCart(cart);

    // Scale background to window size
    sf::Vector2u textureSize = background.getSize();
    sf::Vector2u windowSize = window.getSize();
    float scaleX = static_cast<float>(windowSize.x) / textureSize.x;
    float scaleY = static_cast<float>(windowSize.y) / textureSize.y;
    spriteBackground.setScale(scaleX, scaleY);

    // Set origins and positions so that each sprite is truly centered
    {
        // Character
        sf::FloatRect charBounds = spriteCharacter.getLocalBounds();
        spriteCharacter.setOrigin(charBounds.width / 2.f, charBounds.height / 2.f);
        spriteCharacter.setPosition(118.f, 800.f);

        // Cart
        sf::FloatRect cartBounds = spriteCart.getLocalBounds();
        spriteCart.setOrigin(cartBounds.width / 2.f, cartBounds.height / 2.f);
        spriteCart.setPosition(1550.f, 800.f);

        // Ball
        spriteBall.setScale(0.25f, 0.25f);
        sf::FloatRect ballBounds = spriteBall.getLocalBounds();
        spriteBall.setOrigin(ballBounds.width / 2.f, ballBounds.height / 2.f);
        spriteBall.setPosition(120.f, 549.f);
    }

    float scale = 100.f; // 100 pixels = 1 meter

    // Convert initial positions to meters
    float ballStartX_m = spriteBall.getPosition().x / scale;
    float ballStartY_m = spriteBall.getPosition().y / scale;

    // The basket is at the cart's center for simplicity
    float basketX_m = spriteCart.getPosition().x / scale;
    float basketY_m = spriteCart.getPosition().y / scale;

    // Initial speed and angle
    float initialSpeed = 11.5f;   // m/s
    float initialAngle = 45.f;    // degrees
    float gravity = 9.8f;         // m/s²

    // Create the Ball object
    Ball volleyball(ballStartX_m, ballStartY_m, initialSpeed, initialAngle, gravity, scale);
    sf::Sprite ballSpriteForBall(spriteBall);
    volleyball.setSprite(ballSpriteForBall); // Ball now uses the centered sprite

    // Text for status (goal or no goal)
    sf::Text statusText;
    statusText.setFont(font);
    statusText.setCharacterSize(60);
    statusText.setFillColor(sf::Color::Yellow);
    statusText.setPosition(800.f, 500.f);

    bool simulationRunning = true;
    bool goalScored = false;
    bool outOfBounds = false;

    while (window.isOpen()) {
        float dt = 1.f / 60.f; // fixed timestep

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            else if (event.type == sf::Event::Resized) {
                // Re-scale background
                sf::Vector2u newWindowSize(event.size.width, event.size.height);
                float scaleX = static_cast<float>(newWindowSize.x) / textureSize.x;
                float scaleY = static_cast<float>(newWindowSize.y) / textureSize.y;
                spriteBackground.setScale(scaleX, scaleY);
            }
        }

        if (simulationRunning) {
            // Update physics
            volleyball.update(dt);

            // Check for scoring
            if (volleyball.isScored(basketX_m, basketY_m, 1.1f)) {
                simulationRunning = false;
                goalScored = true;
            }

            // Check out of bounds
            if (volleyball.isOutOfBounds(static_cast<float>(windowSize.x) / scale,
                static_cast<float>(windowSize.y) / scale)) {
                simulationRunning = false;
                outOfBounds = true;
            }
        }

        // Rendering
        window.clear();
        window.draw(spriteBackground);
        window.draw(spriteCharacter);
        window.draw(spriteCart);
        volleyball.draw(window);

        if (!simulationRunning) {
            if (goalScored) {
                statusText.setString("Goal!");
            }
            else if (outOfBounds) {
                statusText.setString("No Goal!");
            }
            window.draw(statusText);
        }

        window.display();
    }
}
