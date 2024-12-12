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

    sf::Sprite spriteCharacter(character);
    sf::Sprite spriteBackground(background);
    sf::Sprite spriteBall(ballTex);
    sf::Sprite spriteCart(cart);

    // Scale background to window
    sf::Vector2u textureSize = background.getSize();
    sf::Vector2u windowSize = window.getSize();
    float scaleX = static_cast<float>(windowSize.x) / textureSize.x;
    float scaleY = static_cast<float>(windowSize.y) / textureSize.y;
    spriteBackground.setScale(scaleX, scaleY);

    // Positions in pixels (from initial code)
    spriteCharacter.setPosition(98.f, 661.f);
    spriteBall.setPosition(120.f, 549.f);
    spriteBall.setScale(0.25f, 0.25f);
    spriteCart.setPosition(1530.f, 690.f);

    // Define scale factor
    float scale = 100.f; // 100 px = 1 m

    // Convert initial positions to meters
    float ballStartX_m = 120.f / scale;
    float ballStartY_m = 549.f / scale;

    float basketX_m = 1530.f / scale; // 15.3 m
    float basketY_m = 690.f / scale;  // 6.9 m

    // Initial speed and angle (could be user input)
    float initialSpeed = 11.5f;      // m/s
    float initialAngle = 45.f;      // degrees
    float gravity = 9.8f;           // m/s²

    // Create the Ball object
    Ball volleyball(ballStartX_m, ballStartY_m, initialSpeed, initialAngle, gravity, scale);

    sf::Sprite ballSpriteForBall(spriteBall);
    // Optionally set origin to the center of the ball for better collision checks:
    sf::FloatRect bounds = ballSpriteForBall.getLocalBounds();
    ballSpriteForBall.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
    volleyball.setSprite(ballSpriteForBall);

    // Text for status (goal or no goal)
    sf::Text statusText;
    statusText.setFont(font);
    statusText.setCharacterSize(60);
    statusText.setFillColor(sf::Color::Yellow);
    statusText.setPosition(800.f, 500.f);

    bool simulationRunning = true;
    bool goalScored = false;
    bool outOfBounds = false;

    sf::Clock clock;

    while (window.isOpen()) {
        float dt = 1.f / 60.f; // fixed timestep

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            else if (event.type == sf::Event::Resized) {
                // Re-scale background if needed
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
            if (volleyball.isScored(basketX_m, basketY_m)) {
                simulationRunning = false;
                goalScored = true;
            }
            // Check out of bounds
            if (volleyball.isOutOfBounds((float)windowSize.x / scale, (float)windowSize.y / scale)) {
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
