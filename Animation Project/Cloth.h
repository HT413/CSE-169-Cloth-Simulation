#ifndef CLOTH_SIM_H
#define CLOTH_SIM_H

#include "Inclusions.h"
#include "Window.hpp"
#include <vector>

// Constants
#define CLOTH_GRID_WIDTH 18 // MUST BE EVEN
#define CLOTH_GRID_HEIGHT 18 // MUST BE EVEN
#define P_SPACE 0.1f // Distance between particles
#define SAMPLES_PER_FRAME 5
#define FLOORSIZE 10.f
#define FLOORHEIGHT -1.f

const float TIMESTEP = 1.f/10000.f;
const vec3 GRAVITY(0.f, -9.8f, 0.f);
const float AIR_DENSITY = 1225.f;
const float DRAG_COEFF = 1.f;
extern vec3 wind;
const float CLOTH_SPRING_FACTOR = 555.5f;
const float CLOTH_DAMP_FACTOR = 5.f;
const float FRICTION = 0.85f;
const float CLOTH_ELASTICITY = 0.05f;

struct Particle{
	vec3 velocity = vec3(0.f, 0.f, 0.f);
	vec3 force =  vec3(0.f, 0.f, 0.f);
	vec3 position;
	GLuint index;
	bool fixed;
	float mass;
	void update();
	void applyForce(vec3);
	void collisionCheck();
};

struct Damper{
	Particle *p1, *p2;
	float length;
	float k;
	float d;
	void computeForce();
};

struct Triangle{
	Particle *p1, *p2, *p3;
	Triangle(Particle * pa1, Particle * pa2, Particle * pa3){
		p1 = pa1; p2 = pa2; p3 = pa3;
	}
	void applyWind();
};

class Cloth
{
private:
	GLuint VAO, VBO, NBO, EBO;
	vector<Particle *> particles;
	vector<Damper *> dampers;
	vector<Triangle *> triangles;
	vector<vec3> positions, normals;
	vector<GLuint> indices;

public:
	Cloth();
	~Cloth();
	void calcNormals();
	void rebind();
	void draw();
	void update();
	void wind();
	// Functions for updating each of the cloth's various components
	void translate(vec3);
	void updateParticles();
	void dampen();
};

#endif