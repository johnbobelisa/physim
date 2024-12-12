#pragma once
#ifndef BALL_H
#define BALL_H

#include <SFML/Graphics.hpp>

class Ball {
public:
    Ball(float x_m, float y_m, float speed_m_s, float angle_degrees, float gravity = 9.8f, float scale = 100.f);

    void update(float dt);
    void draw(sf::RenderWindow& window);
    bool isScored(float basketX_m, float basketY_m, float threshold_m = 0.2f) const;
    bool isOutOfBounds(float maxX_m, float maxY_m) const;

    // Accessors
    float getX_m() const { return x_m; }
    float getY_m() const { return y_m; }

    void setSprite(const sf::Sprite& sprite);

private:
    float x_m, y_m;     // Position in meters
    float vx_m_s, vy_m_s; // Velocity in m/s
    float g;            // Gravity in m/s²
    float scale;        // pixels per meter

    sf::Sprite sprite;
};

#endif
