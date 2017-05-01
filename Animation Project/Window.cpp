#include "Window.hpp"
#include "Camera.h"
#include "Shader.hpp"
#include "Cloth.h"

// Global variables and functions
enum MOUSE_ACTION {M_NONE, C_ROTATE, O_TRANSLATE};
enum WIND_MODIFIER {W_NONE, W_X, W_Y, W_Z};
MOUSE_ACTION mouse_action;
WIND_MODIFIER wind_mod;
GLFWwindow* window; // The main graphics window
int window_w, window_h; // Window width and height
mat4 projection; // Projection and view matrices
double lastX, lastY; // Last cursor x and y coords
GLuint uniformP, uniformV, uniformCamPos; // Projection, view, and camera position uniform locations
GLuint uniformFloor; // Is it the floor or not
Cloth* cloth;
vec3 wind(0.f, 0.f, 0.f);

// Floor properties
GLuint VAO, VBO, NBO;
const GLfloat floorPos[] = {-FLOORSIZE, FLOORHEIGHT, FLOORSIZE, FLOORSIZE, FLOORHEIGHT, FLOORSIZE, 
                            FLOORSIZE, FLOORHEIGHT, -FLOORSIZE, -FLOORSIZE, FLOORHEIGHT, -FLOORSIZE};
const GLfloat floorNormals[] = {0.f, 1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f};

// Shader programs to be used
GLuint basicShader;

void init_window(int w, int h, const char* name){
	// Attempt to initialize GLFW first
	if(!glfwInit()){
		cerr << "Could not initialize GLFW!" << endl;
		PROGERR(51);
	}
	glfwWindowHint(GLFW_SAMPLES, 2); // 2x anti-aliassing
									 // Attempt to make GLFW window
	window = glfwCreateWindow(w, h, name, 0, 0);
	// Check if window creation successful
	if(!window){
		cerr << "Could not create a GLFW window!" << endl;
		glfwTerminate();
		PROGERR(52);
	}
	cout << "Window intiailized successfully!" << endl;
	// Set this window as the OpenGL context
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // Swap every frame
						 // Get width & height of framebuffer to properly resize it
	glfwGetFramebufferSize(window, &w, &h);
	// Update everything now to make sure we can draw properly
	window_resize(window, w, h);
	setLookAt();
}

// Object initialization func
void init_objects(){
	basicShader = LoadShaders("shaders/basic.vert", "shaders/basic.frag");
	glUseProgram(basicShader);
	mouse_action = M_NONE;
	wind_mod = W_NONE;

	cloth = new Cloth();

	// Uniform variables
	uniformP = glGetUniformLocation(basicShader, "projection");
	uniformV = glGetUniformLocation(basicShader, "modelview");
	uniformCamPos = glGetUniformLocation(basicShader, "camPos");
	uniformFloor = glGetUniformLocation(basicShader, "isFloor");

	// Generate the floor
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &NBO);
	// Bind the floor
	glBindVertexArray(VAO);
	// Positions at location 0
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(floorPos), floorPos, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	// Normals at location 1
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, NBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(floorNormals), floorNormals, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	// Done with defining data.
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

// Object deletion func
void clear_objects(){
	glDeleteProgram(basicShader);
	DEL(cloth);
}

// Window resize callback
void window_resize(GLFWwindow* window, int w, int h){
	window_w = w;
	window_h = h;
	// Change the viewport accordingly
	glViewport(0, 0, w, h);
	// Change the projection matrix accordingly
	projection = glm::perspective(M_PI/FLOORSIZE, (float)w / (float)h, 0.1f, 100.f);
}

// Window idle callback
void window_idle(){
	if(cloth) cloth->update();
}

// Window display callback
void window_display(GLFWwindow* window){
	// Check for window events
	glfwPollEvents();
	// Clear window
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	// Update the uniform values
	glUseProgram(basicShader);
	glUniformMatrix4fv(uniformP, 1, GL_FALSE, &(projection[0][0]));
	glUniformMatrix4fv(uniformV, 1, GL_FALSE, &(view[0][0]));
	glUniform3fv(uniformCamPos, 1, &cam_e[0]);
	// Now draw objects
	// Floor
	glUniform1i(uniformFloor, 1);
	glBindVertexArray(VAO);
	glDrawArrays(GL_QUADS, 0, 4);
	glBindVertexArray(0);
	// Cloth
	glUniform1i(uniformFloor, 0);
	if(cloth)
		cloth->draw();
	// Swap buffers now that we're done
	glfwSwapBuffers(window);
}

// Window keyboard callback
void window_key(GLFWwindow* window, int key, int scancode, int action, int mods){
	if(action == GLFW_PRESS){
		switch(key){
		// Modify the wind's x direction when pressing x
		case GLFW_KEY_X:
			cout << "Now modifying wind x direction" << endl;
			wind_mod = W_X;
			break;

		// Modify the wind's y direction when pressing x
		case GLFW_KEY_Y:
			cout << "Now modifying wind y direction" << endl;
			wind_mod = W_Y;
			break;

		// Modify the wind's z direction when pressing x
		case GLFW_KEY_Z:
			cout << "Now modifying wind z direction" << endl;
			wind_mod = W_Z;
			break;

		// Add or subtract
		case GLFW_KEY_EQUAL:
			cout << "Wind direction is now: ";
			if(wind_mod == W_X)
				wind.x += 0.1f;
			else if(wind_mod == W_Y)
				wind.y += 0.1f;
			else if(wind_mod == W_Z)
				wind.z += 0.1f;
			cout << "(" << wind.x << ", " << wind.y << ", " << wind.z << ")" << endl;
			break;

		case GLFW_KEY_MINUS:
			cout << "Wind direction is now: ";
			if(wind_mod == W_X)
				wind.x -= 0.1f;
			else if(wind_mod == W_Y)
				wind.y -= 0.1f;
			else if(wind_mod == W_Z)
				wind.z -= 0.1f;
			cout << "(" << wind.x << ", " << wind.y << ", " << wind.z << ")" << endl;
			break;

		// Kill the wind
		case GLFW_KEY_O:
			cout << "Wind zeroed out" << endl;
			wind = vec3(0.f, 0.f, 0.f);
			break;
			
		// Terminate when Esc pressed
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(window, GL_TRUE);
			break;
		}
	}
}

// Mouse cursor moved callback
void window_cursor(GLFWwindow* window, double x, double y){
	if(mouse_action == C_ROTATE){
		mat4 rotationX = glm::rotate(m_identity, ((float)(lastX - x)) / 120.f, vec3(0.f, 1.f, 0.f));
		cam_e = vec3(rotationX * vec4(cam_e, 1.f));
		cam_up = vec3(rotationX * vec4(cam_up, 1.f));
		vec3 axis = glm::cross((cam_e - cam_d), cam_up);
		mat4 rotationY = glm::rotate(m_identity, ((float)(y - lastY)) / 120.f, axis);
		cam_e = vec3(rotationY * vec4(cam_e, 1.f));
		cam_up = vec3(rotationY * vec4(cam_up, 1.f));
		setLookAt();
		// Done rotating, update x and y
		lastX = x;
		lastY = y;
	}
	else if(mouse_action == O_TRANSLATE){
		// Derive the positive x in the current perspective view
		glm::vec3 pos_x = glm::cross(cam_up, (cam_e - cam_d));
		// Calculate the translation matrix
		glm::vec3 shiftAmount = ((float)(x - lastX) / 4200.0f) * pos_x + ((float)(lastY - y) / 180.0f) * cam_up;
		// Now shift the cloth up this much
		cloth->translate(shiftAmount);
		// Done translating, update x and y
		lastX = x;
		lastY = y;
	}
}

// Mouse button callback
void window_mouse(GLFWwindow* window, int button, int action, int mods){
	// Mouse released, do nothing
	if(action == GLFW_RELEASE){
		mouse_action = M_NONE;
	}
	// Mouse pressed, determine appropriate action
	else{
		glfwGetCursorPos(window, &lastX, &lastY);
		if(button == GLFW_MOUSE_BUTTON_1){
			mouse_action = C_ROTATE;
		}
		else if(button = GLFW_MOUSE_BUTTON_2){
			mouse_action = O_TRANSLATE;
		}
	}
}

// Mouse scroll callback
void window_scroll(GLFWwindow* window, double dx, double dy){
	// Determine if zoom or not
	(dy > 0.0)? cam_e *= 0.98f : cam_e *= 1.02f;
	setLookAt();
}