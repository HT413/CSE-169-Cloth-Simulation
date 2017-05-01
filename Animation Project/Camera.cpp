#include "Camera.h"
#include "Window.hpp"

vec3 cam_e(0.f, 0.f, 15.f);
vec3 cam_d(0.f, 0.f, 0.f);
vec3 cam_up(0.f, 1.f, 0.f);
mat4 view(1.0f);

void setLookAt(){
	view = glm::lookAt(cam_e, cam_d, cam_up);
}