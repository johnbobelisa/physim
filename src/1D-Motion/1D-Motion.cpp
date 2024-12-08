#include "1D-Motion.h"

OneDimensionalMotion::OneDimensionalMotion(sf::RenderWindow& wnd)
    : window(wnd),
    particle(sf::Vector2f(400.f, 100.f)), // Initial position
    floorY(550.f),
    gravity(0.f, 98.f),
    radius(10.f),
    initialVelocity(0.f)
{
    shape.setRadius(radius);
    shape.setOrigin(radius, radius);
    shape.setFillColor(sf::Color::Red);
}

void OneDimensionalMotion::setupScenario() {
    // Set up initial conditions for the scenario.
    // For example, set initial velocity to demonstrate different subtopics.
    // This could be a menu-driven choice in main.cpp.
    particle.setVelocity(sf::Vector2f(0.f, initialVelocity));
    particle.setAcceleration(sf::Vector2f(0.f, 0.f));
}

void OneDimensionalMotion::update(float dt) {
    // Apply gravity if the subtopic requires it
    particle.applyForce(gravity * particle.getMass());

    // Update particle motion
    particle.update(dt);

    // Handle bouncing or stopping at floor
    sf::Vector2f pos = particle.getPosition();
    if (pos.y + radius >= floorY) {
        sf::Vector2f vel = particle.getVelocity();
        vel.y = -vel.y * 0.8f; // Simulate bounce with reduced velocity
        particle.setVelocity(vel);
        particle.setPosition(sf::Vector2f(pos.x, floorY - radius));
    }
}

void OneDimensionalMotion::draw() {
    shape.setPosition(particle.getPosition());

    // Draw particle
    window.draw(shape);

    // Draw floor line
    sf::RectangleShape floor(sf::Vector2f((float)window.getSize().x, 5.f));
    floor.setPosition(0.f, floorY);
    floor.setFillColor(sf::Color::Black);
    window.draw(floor);
}
