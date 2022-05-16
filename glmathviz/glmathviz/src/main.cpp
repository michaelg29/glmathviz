#include <iostream>
#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "rendering/shader.h"

#include "programs/rectangle.hpp"
#include "programs/sphere.hpp"

#include "io/camera.h"
#include "io/keyboard.h"
#include "io/mouse.h"

std::string Shader::defaultDirectory = "assets/shaders";

// initialization methods
void initGLFW(unsigned int versionMajor, unsigned int versionMinor);
void createWindow(GLFWwindow*& window,
	const char* title, unsigned int width, unsigned int height,
	GLFWframebuffersizefun framebufferSizeCallback);

// callback signatures
void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void processInput(double dt);
void updateCameraMatrices();
void keyChanged(GLFWwindow* window, int key, int scancode, int action, int mods);
void cursorChanged(GLFWwindow* window, double _x, double _y);
void mouseButtonChanged(GLFWwindow* window, int button, int action, int mods);
void scrollChanged(GLFWwindow* window, double dx, double dy);

// GLOBAL VARIABLES
int scr_width = 800, scr_height = 800;
GLFWwindow* window = nullptr;
bool re_render = true;

// CAMERA
Camera cam(glm::vec3(-2.0f, 0.0f, 0.0f));
glm::mat4 view;
glm::mat4 projection;

// GLOBAL PROGRAMS
std::vector<Program*> programs;
Rectangle rect;
Sphere sphere(10);

int main() {
	std::cout << "Hello, math!" << std::endl;

	// initialize
	initGLFW(3, 3);
	createWindow(window, "GLMathViz", scr_width, scr_height, framebufferSizeCallback);
	if (!window) {
		std::cout << "Could not create window" << std::endl;
		glfwTerminate();
		return -1;
	}

	// load glad
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Could not load GLAD" << std::endl;
		glfwTerminate();
		return -1;
	}

	// window parameters
	glfwSwapInterval(1); // v-sync
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// rendering parameters
	glEnable(GL_DEPTH_TEST);
	glViewport(0, 0, scr_width, scr_height);

	// setup IO callbacks
	glfwSetKeyCallback(window, Keyboard::keyCallback);
	glfwSetCursorPosCallback(window, Mouse::cursorPosCallback);
	glfwSetMouseButtonCallback(window, Mouse::mouseButtonCallback);
	glfwSetScrollCallback(window, Mouse::mouseWheelCallback);
	Keyboard::keyCallbacks.push_back(keyChanged);
	Mouse::cursorPosCallbacks.push_back(cursorChanged);
	Mouse::mouseButtonCallbacks.push_back(mouseButtonChanged);
	Mouse::mouseWheelCallbacks.push_back(scrollChanged);

	// generate instances
	sphere.addInstance(glm::vec3(1.0f), glm::vec3(0.2f));
	programs.push_back(&sphere);
	programs.push_back(&rect);

	// setup programs
	for (Program* program : programs)
	{
		program->load();
	}

	// timing variables
	double dt = 0.0;
	double lastFrame = 0.0;

	updateCameraMatrices();

	while (!glfwWindowShouldClose(window)) {
		// update time
		dt = glfwGetTime() - lastFrame;
		lastFrame += dt;

		// input
		glfwWaitEventsTimeout(0.001);
		processInput(dt);

		// update
		for (Program* program : programs)
		{
			if (program->update(dt))
			{
				re_render = true;
			}
		}

		// rendering
		if (re_render) {
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// render programs
			for (Program* program : programs)
			{
				program->render();
			}

			// move rendered buffer to screen
			glfwSwapBuffers(window);

			re_render = false;
		}
	}

	// cleanup programs
	for (Program* program : programs)
	{
		program->cleanup();
	}

	programs.clear();

	// terminate
	glfwTerminate();

	return 0;
}

void initGLFW(unsigned int versionMajor, unsigned int versionMinor) {
	glfwInit();

	// pass in parameters
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, versionMajor);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, versionMinor);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

void createWindow(GLFWwindow*& window,
	const char* title, unsigned int width, unsigned int height,
	GLFWframebuffersizefun framebufferSizeCallback) {
	window = glfwCreateWindow(width, height, title, NULL, NULL);
	if (!window) {
		return;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	scr_width = width;
	scr_height = height;
	updateCameraMatrices();
}

void processInput(double dt) {
	if (Keyboard::key(GLFW_KEY_W)) {
		cam.updateCameraPos(CameraDirection::FORWARD, dt);
		updateCameraMatrices();
	}
	if (Keyboard::key(GLFW_KEY_S)) {
		cam.updateCameraPos(CameraDirection::BACKWARD, dt);
		updateCameraMatrices();
	}
	if (Keyboard::key(GLFW_KEY_D)) {
		cam.updateCameraPos(CameraDirection::RIGHT, dt);
		updateCameraMatrices();
	}
	if (Keyboard::key(GLFW_KEY_A)) {
		cam.updateCameraPos(CameraDirection::LEFT, dt);
		updateCameraMatrices();
	}
	if (Keyboard::key(GLFW_KEY_SPACE)) {
		cam.updateCameraPos(CameraDirection::UP, dt);
		updateCameraMatrices();
	}
	if (Keyboard::key(GLFW_KEY_LEFT_SHIFT)) {
		cam.updateCameraPos(CameraDirection::DOWN, dt);
		updateCameraMatrices();
	}
}

void updateCameraMatrices() {
	view = cam.getViewMatrix();
	projection = glm::perspective(
		glm::radians(cam.getZoom()),			// FOV
		(float)scr_width / (float)scr_height,	// aspect ratio
		0.1f, 100.0f							// near/far bounds
	);

	re_render = true;

	rect.updateCameraMatrices(projection * view, cam.cameraPos);
	sphere.updateCameraMatrices(projection * view, cam.cameraPos);
}

void keyChanged(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (Keyboard::key(GLFW_KEY_ESCAPE)) {
		glfwSetWindowShouldClose(window, true);
	}
}

void cursorChanged(GLFWwindow* window, double _x, double _y) {
	// set camera direction
	double dx = Mouse::getDX();
	double dy = Mouse::getDY();
	if (dx != 0 || dy != 0) {
		cam.updateCameraDirection(dx, dy);
	}

	updateCameraMatrices();
}

void mouseButtonChanged(GLFWwindow* window, int button, int action, int mods) {

}

void scrollChanged(GLFWwindow* window, double dx, double dy) {
	// set camera zoom
	double scrollDy = Mouse::getScrollDY();
	if (scrollDy != 0) {
		cam.updateCameraZoom(scrollDy);
	}

	updateCameraMatrices();
}