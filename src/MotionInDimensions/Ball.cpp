#include "Ball.h"
#include <cmath>

Ball::Ball(float x_m, float y_m, float speed_m_s, float angle_degrees, float gravity, float scale)
    : x_m(x_m), y_m(y_m), g(gravity), scale(scale)
{
    float angle_rad = angle_degrees * 3.14159f / 180.f;
    vx_m_s = speed_m_s * std::cos(angle_rad);
    vy_m_s = -speed_m_s * std::sin(angle_rad); // negative for upward initial motion
}

void Ball::update(float dt) {
    // Update velocities
    vy_m_s += g * dt;

    // Update positions
    x_m += vx_m_s * dt;
    y_m += vy_m_s * dt;

    // Update sprite position
    sprite.setPosition(x_m * scale, y_m * scale);
}

void Ball::draw(sf::RenderWindow& window) {
    window.draw(sprite);
}

bool Ball::isScored(float basketX_m, float basketY_m, float threshold_m) const {
    float dx = x_m - basketX_m;
    float dy = y_m - basketY_m;
    float dist = std::sqrt(dx * dx + dy * dy);
    return dist < threshold_m;
}

bool Ball::isOutOfBounds(float maxX_m, float maxY_m) const {
    // If the ball goes beyond any boundary, consider it out of bounds.
    return (x_m < 0.f || x_m > maxX_m || y_m < 0.f || y_m > maxY_m);
}

void Ball::setSprite(const sf::Sprite& spr) {
    sprite = spr;
    sprite.setOrigin(sprite.getTexture()->getSize().x / 2, sprite.getTexture()->getSize().y / 2);  // center
}
