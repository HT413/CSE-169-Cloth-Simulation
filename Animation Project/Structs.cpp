#include "Cloth.h"

// This file defines the functions for the particle, damper, and triangle structs

// Particle update function
void Particle::update(){
	// Do nothing if fixed
	if(fixed) return;
	// Otherwise update
	force += mass * GRAVITY;
	velocity += TIMESTEP * (force / mass);
	position += TIMESTEP * velocity;
	collisionCheck();
	force = vec3(0.f, 0.f, 0.f);
}

// Particle collision check function. Checks for collision with ground
void Particle::collisionCheck(){
	if(position.y < FLOORHEIGHT){
		position.y = 2 * FLOORHEIGHT - position.y;
		velocity.y = -CLOTH_ELASTICITY * velocity.y;
		velocity.x = (1.f - FRICTION) * velocity.x;
		velocity.z = (1.f - FRICTION) * velocity.z;
	}
}

// Particle apply force function. Applies a small force to it for displacement later in update.
void Particle::applyForce(vec3 f){
	force += f;
}

// Spring damper force compute function
void Damper::computeForce(){
	vec3 direction = p2->position - p1->position;
	vec3 dir = glm::normalize(direction);
	// TODO add dampening force
	float v1 = glm::dot(dir, p1->velocity);
	float v2 = glm::dot(dir, p2->velocity);
	float springForce = -k * (length - glm::length(direction)) - d * (v1 - v2);
	vec3 particleForce = springForce * dir;
	p1->applyForce(particleForce);
	p2->applyForce(-particleForce);
}

// Triangle force compute function
void Triangle::applyWind(){
	vec3 velocity = (p1->velocity + p2->velocity + p3->velocity)/3.f - wind;
	vec3 normal = glm::cross((p2->position - p1->position), (p3->position - p1->position));
	float area = glm::length(normal);
	normal = glm::normalize(normal) * glm::dot(glm::normalize(velocity), normal);
	vec3 force = -0.5f*AIR_DENSITY*DRAG_COEFF*area*powf(glm::length(velocity), 2)*normal/3.f;
	// Apply forces to the vertices
	p1->applyForce(force);
	p2->applyForce(force);
	p3->applyForce(force);
}