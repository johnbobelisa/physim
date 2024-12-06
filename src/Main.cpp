#include <SFML/Graphics.hpp>
#include "Particle.h"

int main() {
    // Create a window where we'll draw everything.
    // sf::VideoMode(800, 600) means window is 800 pixels wide and 600 pixels high.
    // "Particle Demo" is the title of the window.
    // sf::RenderWindow is a class, and we are creating an instance called "window".
    sf::RenderWindow window(sf::VideoMode(800, 600), "Particle Demo");

    // Create a Particle object at position (400.f, 100.f).
    // "f" after a number just means it's a float (a decimal number type).
    Particle p(sf::Vector2f(400.f, 100.f));

    // Create a vector for gravity.
    // This means gravity pushes objects down (0 on x, 98 on y).
    // Think of it as "acceleration due to gravity".
    sf::Vector2f gravity(0.f, 100.f);

    float radius = 10.f; // The radius of the circle we'll use to show the particle on screen.
    sf::CircleShape shape(radius); // sf::CircleShape is a class representing a circle.
    shape.setOrigin(radius, radius);
    // setOrigin moves the "reference point" of the shape. 
    // By default, shapes are drawn from their top-left corner.
    // If we set the origin to (radius, radius), we make the center of the circle be its origin.
    // This means if we put the circle at (400,300), it will be perfectly centered at that point.

    shape.setFillColor(sf::Color::Red);
    // setFillColor changes the inside color of the shape. sf::Color::Red is a predefined color.

    sf::Clock clock;
    // sf::Clock is a class from SFML that lets us measure time.
    // We'll use it to measure how much time passes between frames (dt).

    float floorY = 550.f;
    // We'll pretend there's a floor at y=550 (somewhere near the bottom of the window).
    // If the particle hits this line, it'll bounce.

    while (window.isOpen()) {
        // This loop runs until we close the window.
        // Each loop is one "frame".

        sf::Event event;
        while (window.pollEvent(event)) {
            // pollEvent checks if there are any events (like closing the window or pressing keys)
            // If the user clicked the close button, we close the window.
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        // Restart the clock and get the time passed since last frame in seconds.
        // This helps us move the particle smoothly even if the computer is slow or fast.
        float dt = clock.restart().asSeconds();

        // Apply gravity to the particle.
        // We do gravity * mass because Force = mass * acceleration.
        p.applyForce(gravity * p.getMass());

        // Update the particle's position and velocity based on how much time passed (dt).
        p.update(dt);

        // Check if the particle is hitting the floor.
        // p.getPosition() gives us the particle's current position as a vector (x,y).
        // If the particle's bottom (position.y + radius) is at or below the floor (floorY),
        // it means it hit the floor.
        sf::Vector2f pos = p.getPosition();
        if (pos.y + radius >= floorY) {
            // Bounce the particle by reversing its vertical velocity.
            // p.getVelocity() gets the current velocity.
            sf::Vector2f vel = p.getVelocity();
            vel.y = -vel.y * 0.8f;
            // Multiplying by 0.8f reduces the speed so it doesn't bounce forever.
            p.setVelocity(vel);

            // Move the particle just above the floor so it doesn't "sink" below.
            p.setPosition(sf::Vector2f(pos.x, floorY - radius));
        }

        // Before drawing, clear the window with a white background.
        window.clear(sf::Color::White);

        // Put the shape at the particle's position.
        shape.setPosition(p.getPosition());
        // Now the shape will be drawn where the particle is.

        // Draw the particle (the red circle).
        window.draw(shape);

        // Draw the floor as a thin black rectangle at y=550.
        sf::RectangleShape floor(sf::Vector2f(800.f, 5.f));
        floor.setPosition(0.f, floorY);
        floor.setFillColor(sf::Color::Black);
        // This means we have a black line across the screen at y=550.
        window.draw(floor);

        // Show everything we've drawn on the screen.
        window.display();
    }

    return 0; // Return 0 means the program ended successfully.
}