#include "ProjectileMotion.h"
#include <SFML/Graphics.hpp>
#include <cmath>

// This simulation shows a projectile fired from the bottom-left corner of the window.
// It moves under gravity and we show its trajectory.

void runProjectileMotionSimulation() {
    sf::RenderWindow window(sf::VideoMode(1280, 720), "Projectile Motion Simulation", sf::Style::Close);
    window.setFramerateLimit(60);

    // Initial conditions
    float initialSpeed = 50.0f;    // pixels per second
    float angleDegrees = 45.0f;     // launch angle
    float gravity = 9.8f;           // pixels per second^2, adjust for realism
    sf::Vector2f startPosition(50.0f, 650.0f); // starting near bottom-left

    // Convert angle to radians
    float angleRad = angleDegrees * 3.14159f / 180.0f;

    // Initial velocity components
    float vx = initialSpeed * std::cos(angleRad);
    float vy = -initialSpeed * std::sin(angleRad);
    // Negative vy because in SFML, positive y goes down the screen.

    // Shape representing the projectile
    sf::CircleShape projectile(10.0f);
    projectile.setFillColor(sf::Color::Red);
    projectile.setPosition(startPosition);

    // To visualize the trajectory, we will store positions in a vertex array
    sf::VertexArray trajectory(sf::LineStrip);
    trajectory.append(sf::Vertex(startPosition, sf::Color::Yellow));

    // Time management
    sf::Clock clock;
    float elapsedTime = 0.0f;

    bool running = true;

    while (window.isOpen() && running) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                return;
            }

            if (event.type == sf::Event::KeyPressed) {
                // Press Escape to quit simulation
                if (event.key.code == sf::Keyboard::Escape) {
                    running = false;
                }
            }
        }

        float dt = clock.restart().asSeconds();
        elapsedTime += dt;

        // Update projectile position
        // Position equations:
        // x(t) = x0 + vx * t
        // y(t) = y0 + vy * t + 0.5 * g * t^2
        // Note: since downward is positive, gravity adds to y over time.
        float x = startPosition.x + vx * elapsedTime;
        float y = startPosition.y + vy * elapsedTime + 0.5f * gravity * elapsedTime * elapsedTime;

        projectile.setPosition(x, y);

        // Add this position to the trajectory
        trajectory.append(sf::Vertex(sf::Vector2f(x, y), sf::Color::Yellow));

        // If the projectile goes beyond window limits or hits the "ground" (bottom)
        if (y > 700.0f || x > 1300.0f) {
            running = false;
        }

        // Rendering
        window.clear(sf::Color::Black);
        window.draw(trajectory);
        window.draw(projectile);
        window.display();
    }

    window.close();
}
