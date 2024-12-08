#pragma once

#include <SFML/System.hpp> // for sf::Vector2f (2d vector)

class Particle {
	public:
		// Particle constructor (runs when a particle is created)
		Particle(
			const sf::Vector2f& pos = sf::Vector2f(0.f, 0.f),
			const sf::Vector2f& vel = sf::Vector2f(0.f, 0.f),
			float mass = 1.f
		);

		// "update" will move the particle based on its velocity and acceleration
		void update(float dt);

		// "applyForce" applies force to the particle (like gravity, etc)
		void applyForce(const sf::Vector2f &force);

		// Set the particle's position, velocity, acceleration, and mass
		void setPosition(const sf::Vector2f &pos);
		void setVelocity(const sf::Vector2f &vel);
		void setAcceleration(const sf::Vector2f &acc);
		void setMass(float m);

		const sf::Vector2f& getPosition() const;
		const sf::Vector2f& getVelocity() const;
		const sf::Vector2f& getAcceleration() const;
		float getMass() const;

	private:
		// These are the "parts" of a Particle:
		sf::Vector2f position;
		sf::Vector2f velocity;
		sf::Vector2f acceleration;
		float mass;
};

