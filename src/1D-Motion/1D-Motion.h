#pragma once
#include <SFML/Graphics.hpp>
#include "Particle.h"

class OneDimensionalMotion {
public:
    OneDimensionalMotion(sf::RenderWindow& window);
    void setupScenario();
    void update(float dt);
    void draw();

private:
    sf::RenderWindow& window;
    Particle particle;       // Reusing your Particle class
    sf::CircleShape shape;   // Visual representation of the particle
    float floorY;            // "Floor" level

    // Example parameters you can tweak:
    sf::Vector2f gravity;
    float radius;
    float initialVelocity;    // For certain subtopics, you might vary this
};
