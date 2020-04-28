#include "boid.h"

// Constuctors
// =====================================

// Defualt
Boid::Boid() {
	position = { 0.0f,0.0f };
	velocity = { 0.0f,0.0f };
	acceleration = { 0.0f,0.0f };
	colour = olc::RED;
	angle = 0;
}

// Paramaterized
Boid::Boid(olc::vf2d position, float angle, olc::Pixel colour) {
	this->position = { position.x,position.y };
	velocity = { cos(angle), sin(angle) };
	acceleration = { 0,0 };
	this->colour = colour;
	this->angle = angle;
}

// "Physics" Systems
// =====================================
void Boid::update(float f) {
	velocity += acceleration;
	if (velocity.mag() != 0) 
		position += velocity.norm() * f * maxSpeed;
	acceleration *= 0;
	angle = calcAngle();
}

void Boid::applyForce(olc::vf2d force) {
	acceleration += force;
}

// Forces
// =====================================

// Seek a point
olc::vf2d Boid::seek(olc::vi2d target) {
	olc::vf2d desired = target - position;
	if (desired.mag() != 0) {
		desired = desired.norm();
		desired *= maxSpeed;
	}
	return this->steer(desired);
}

// Steer towards a desired velocity
olc::vf2d Boid::steer(olc::vi2d desired) {
	olc::vf2d steer = desired - velocity;
	if (steer.mag() != 0 && steer.mag() > maxForce) {
		steer = steer.norm();
		steer *= maxForce;
	}
	return steer;
}

// MISC
// =====================================
// Calculate the angle of a boid
float Boid::calcAngle() {
	if (velocity.mag() != 0)
		return atan2(velocity.x - 0, 0 - velocity.y);
	return angle;
}