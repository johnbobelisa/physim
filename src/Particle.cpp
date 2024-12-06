#include "Particle.h"

Particle::Particle(const sf::Vector2f& pos, const sf::Vector2f& vel, float m) :
	position(pos), velocity(vel), mass(m) {
		// The constructor sets the particle’s starting position, velocity, and mass
		// acceleration starts at (0,0) by default
}

void Particle::update(float dt) {
	// This function moves the particle over a step of time (dt)

	// velocity = velocity + acceleration * dt
	velocity += acceleration * dt;

	// position = position + velocity * dt
	position += velocity * dt;

	// After we move, we can reset acceleration if we want so that
	// each frame we can apply new forces
	 
	acceleration = sf::Vector2f(0.f, 0.f);
}

void Particle::applyForce(const sf::Vector2f &force) {
	// Force changes acceleration
	// a = F/m

	acceleration += force / mass;
}

// Set parameter functions
void Particle::setPosition(const sf::Vector2f& pos) {
	position = pos;
}

void Particle::setVelocity(const sf::Vector2f& vel) {
	velocity = vel;
}

void Particle::setAcceleration(const sf::Vector2f& acc) {
	acceleration = acc;
}

void Particle::setMass(float m) {
	mass = m;
}

const sf::Vector2f& Particle::getPosition() const {
	return position;
}

const sf::Vector2f& Particle::getVelocity() const {
	return velocity;
}

const sf::Vector2f& Particle::getAcceleration() const {
	return acceleration;
}

float Particle::getMass() const {
	return mass;
}