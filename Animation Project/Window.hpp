#ifndef WINDOW_HPP
#define WINDOW_HPP

#include "Inclusions.h"

// Global variables and functions
extern GLFWwindow* window; // The main graphics window
extern int window_w, window_h; // Window width and height
extern vec3 cam_e, cam_d, cam_up; // Camera vectors
extern mat4 projection, view; // Projection and view matrices
extern GLuint uniformP, uniformV, uniformCamPos; // Projection, view, and camera position uniform locations

void init_window(int, int, const char*);
void init_objects();
void clear_objects();
void window_resize(GLFWwindow*, int, int);
void window_idle();
void window_display(GLFWwindow*);
void window_key(GLFWwindow*, int, int, int, int);
void window_cursor(GLFWwindow*, double, double);
void window_mouse(GLFWwindow*, int, int, int);
void window_scroll(GLFWwindow*, double, double);

#endif