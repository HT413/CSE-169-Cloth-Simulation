#include "Cloth.h"

// Ctor func. Create the grid of particles and set up the buffer objects accordingly.
Cloth::Cloth(){
	cout << "Generating cloth..." << endl;
	// Generate the grid of points
	unsigned int ind = 0;
	for(int i = 0; i < CLOTH_GRID_HEIGHT; i++){
		// Start from bottom level and move up, move left to right
		for(int j = 0; j < CLOTH_GRID_WIDTH; j++){
			Particle *p = new Particle();
			p->position = vec3(P_SPACE * (((float) j) - ((float) CLOTH_GRID_WIDTH)/2.0f), 0.5f + P_SPACE * i, 0.f);
			p->mass = 0.1f;
			p->index = ind++;
			(i == CLOTH_GRID_HEIGHT - 1)? p->fixed = true : p->fixed = false;
			particles.push_back(p);
			positions.push_back(p->position);
			normals.push_back(vec3(0.f, 0.f, 0.f));
		}
	}
	// Generate the spring dampers
	for(int i = 0; i < CLOTH_GRID_HEIGHT; i++){
		// Also start from bottom level and move up, left to right
		for(int j = 0; j < CLOTH_GRID_WIDTH; j++){
			// Create dampers connecting to the 3 points above and point to the right if exists
			// First connect with upper level if exists
			if(i < CLOTH_GRID_HEIGHT - 1){
				// Check upper left
				if(j > 0){
					Damper *d1 = new Damper();
					d1->p1 = particles[j + i * CLOTH_GRID_HEIGHT];
					d1->p2 = particles[j - 1 + (i + 1) * CLOTH_GRID_HEIGHT];
					d1->length = glm::length(d1->p1->position - d1->p2->position);
					d1->k = CLOTH_SPRING_FACTOR;
					d1->d = CLOTH_DAMP_FACTOR;
					dampers.push_back(d1);
				}
				// Check upper right
				if(j < CLOTH_GRID_WIDTH - 1){
					Damper *d2 = new Damper();
					d2->p1 = particles[j + i * CLOTH_GRID_HEIGHT];
					d2->p2 = particles[j + 1 + (i + 1) * CLOTH_GRID_HEIGHT];
					d2->length = glm::length(d2->p1->position - d2->p2->position);
					d2->k = CLOTH_SPRING_FACTOR;
					d2->d = CLOTH_DAMP_FACTOR;
					dampers.push_back(d2);
				}
				// Upper always
				Damper *d3 = new Damper();
				d3->p1 = particles[j + i * CLOTH_GRID_HEIGHT];
				d3->p2 = particles[j + (i + 1) * CLOTH_GRID_HEIGHT];
				d3->length = glm::length(d3->p1->position - d3->p2->position);
				d3->k = CLOTH_SPRING_FACTOR;
				d3->d = CLOTH_DAMP_FACTOR;
				dampers.push_back(d3);
			}
			// Now just check right
			if(j < CLOTH_GRID_WIDTH - 1){
				Damper *d4 = new Damper();
				d4->p1 = particles[j + i * CLOTH_GRID_HEIGHT];
				d4->p2 = particles[j + 1 + i * CLOTH_GRID_HEIGHT];
				d4->length = glm::length(d4->p1->position - d4->p2->position);
				d4->k = CLOTH_SPRING_FACTOR;
				d4->d = CLOTH_DAMP_FACTOR;
				dampers.push_back(d4);
			}
		}
	}

	// Generate the indices array
	for(int i = 0; i < CLOTH_GRID_HEIGHT - 1; i++){
		for(int j = 0; j < CLOTH_GRID_WIDTH - 1; j++){
			int i1 = j + i * CLOTH_GRID_HEIGHT;
			int i2 = (j + 1) + i * CLOTH_GRID_HEIGHT;
			int i3 = j + (i + 1) * CLOTH_GRID_HEIGHT;
			indices.push_back(i1);
			indices.push_back(i2);
			indices.push_back(i3);
			triangles.push_back(new Triangle(particles[i1], particles[i2], particles[i3]));
			i1 = (j + 1) + i * CLOTH_GRID_HEIGHT;
			i2 = (j + 1) + (i + 1) * CLOTH_GRID_HEIGHT;
			i3 = j + (i + 1) * CLOTH_GRID_HEIGHT;
			indices.push_back(i1);
			indices.push_back(i2);
			indices.push_back(i3);
			triangles.push_back(new Triangle(particles[i1], particles[i2], particles[i3]));
		}
	}
	// Calculate the normals
	calcNormals();

	// Now bind everything
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &NBO);
	glGenBuffers(1, &EBO);
	// Now set up the VAO and its buffers
	glBindVertexArray(VAO);
	// Positions at location 0
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(vec3), &positions[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	// Normals at location 1
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, NBO);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(vec3), &normals[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	// Indices for the triangles
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);
	// Done with defining data.
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	cout << "All done!" << endl;
}

// Dtor func. Destroy the VAO and buffer objects
Cloth::~Cloth(){
	particles.clear();
	normals.clear();
	triangles.clear();
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &NBO);
	glDeleteBuffers(1, &EBO);
}

// Rebind func. Binds the updated list of particles and normals.
void Cloth::rebind(){
	glBindVertexArray(VAO);
	// Rebind vertices
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(vec3), &positions[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	// Rebind normals
	glBindBuffer(GL_ARRAY_BUFFER, NBO);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(vec3), &normals[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	// Done with modifications
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

// This function calculates the normals.
void Cloth::calcNormals(){
	for(vec3 v: normals){
		v = vec3(0.f, 0.f, 0.f);
	}
	for(Triangle *t: triangles){
		Particle* a = t->p1, *b = t->p2, *c = t->p3;
		vec3 norm = glm::normalize(glm::cross((b->position - a->position), (c->position - a->position)));
		normals[a->index] += norm;
		normals[b->index] += norm;
		normals[c->index] += norm;
	}
}

// Update func. Determines how to update the cloth.
void Cloth::update(){
	for(int i = 0; i < SAMPLES_PER_FRAME; i++){
		updateParticles();
		dampen();
		wind();
		// Recalculate normals
		calcNormals();
	}
	// Rebind everything now that we're done
	rebind();
}

// Translate func. Moves the cloth's fixed points
void Cloth::translate(vec3 amt){
	for(Particle *p: particles){
		if(p->fixed){
			p->position += amt;
			// Prevent the fixed points from going through the floor
			if(p->position.y <= FLOORHEIGHT)
				p->position -= amt;
		}
	}
}

// updateParticles func. Updates particles by applying various forces to it.
void Cloth::updateParticles(){
	for(Particle * p: particles){
		p->update();
		positions[p->index] = p->position;
	}
}

// Dampen func. Applies the forces for the spring dampers
void Cloth::dampen(){
	for(Damper * d: dampers){
		d->computeForce();
	}
}

// Wind func. Applies aerodynamic force to the triangles
void Cloth::wind(){
	for(Triangle * t: triangles){
		t->applyWind();
	}
}

// Draw func. Simply draws the cloth.
void Cloth::draw(){
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}