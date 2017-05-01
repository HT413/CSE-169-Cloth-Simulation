// Common inclusions and definitions for the project
#ifndef GLOBAL_INCLUDES
#define GLOBAL_INCLUDES

// This file defines the window behaviors
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#ifndef GLM_FORCE_RADIANS
#define GLM_FORCE_RADIANS
#endif
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

// Definitions and constants
#define PROGERR(x) getchar();exit(x)
#define DEL(x) if(x)delete(x)

const float M_PI = 3.14159265f;
typedef glm::mat4 mat4;
typedef glm::vec3 vec3;
typedef glm::vec4 vec4;
const mat4 m_identity(1.f);

using namespace std;

#endif