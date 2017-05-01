/*
* This is the main program file for the project.
* Creates a GLFW window and runs the graphics application.
* Written by Hoang Tran, January 2017.
*/

#include "Window.hpp"

// Define the glfw window callback functions
void set_callbacks(){
	// Window resize callback
	glfwSetFramebufferSizeCallback(window, window_resize);
	// Keyboard callback
	glfwSetKeyCallback(window, window_key);
	// Mouse cursor callback
	glfwSetCursorPosCallback(window, window_cursor);
	// Mouse button callback
	glfwSetMouseButtonCallback(window, window_mouse);
	// Mouse scroll callback
	glfwSetScrollCallback(window, window_scroll);
}

// Set up GLew
void init_glew(){
	// Attempt to intialize glew
	GLenum err = glewInit();
	if(err != GLEW_OK){
		cerr << "GLEW error: " << glewGetErrorString(err) << endl;
		glfwTerminate();
		PROGERR(53);
	}
	cout << "GLEW initialized. Using version " << glewGetString(GLEW_VERSION) << endl;
}

// Set up GLew and define the OpenGL settings
void set_OpenGL(){
	// Attempt to initialize GLew first
	init_glew();
	// GLew initialized, now set up OpenGL settings
	glEnable(GL_DEPTH_TEST); // Depth test enable
	glDepthFunc(GL_LEQUAL); // Depth test func
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Fill polygons
	glDisable(GL_CULL_FACE); // Disable face culling
	glClearColor(0.1f, 0.1f, 0.1f, 1.f);
	// Done setting up, inform the user
	cout << "Graphics hardware: " << glGetString(GL_RENDERER) << endl;
	cout << "OpenGL version: " << glGetString(GL_VERSION) << endl;
	cout << "Shader version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
}

int main(){
	// Create GLFW window
	init_window(800, 600, "CSE 169 Project 1");
	// Set the window callbacks
	set_callbacks();
	// Set the OpenGL settings
	set_OpenGL();
	// All done setting up, now intialize objects and go to the program main loop
	init_objects();
	while(!glfwWindowShouldClose(window)){
		window_display(window);
		window_idle();
	}
	// Program termination
	clear_objects();
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}