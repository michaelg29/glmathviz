#include <iostream>
#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "rendering/shader.h"
#include "rendering/uniformmemory.hpp"

#include "programs/arrow.hpp"
#include "programs/rectangle.hpp"
#include "programs/sphere.hpp"
#include "programs/surface.hpp"
#include "programs/path.hpp"
#include "programs/stereographic_proj.hpp"
#include "programs/grid.hpp"

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
Arrow arrow(5);
Surface surface(5, 500, 500);
glm::vec3 func(double t) {
	return { 0.0, 3*cos(t) - 3.0, sin(t) };
}
ParametrizedPath* transitionPath = new ParametrizedPath(
	func,
	0.0, glm::two_pi<double>(),
	2.0
	);
Sphere sphere(transitionPath, 10);
Path path(transitionPath, 200);
StereographicProj stereo;
Grid grid(glm::ivec2(-10, -10), glm::ivec2(10, 10));

typedef struct {
	glm::vec3 dir;
	glm::vec4 ambient;
	glm::vec4 diffuse;
	glm::vec4 specular;
} DirLight;

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
	arrow.addInstance(glm::vec3(0.0f), glm::vec3(1.0f, 0.0f, 0.0f), 0.0125f, 0.025f, 0.15f, Material::red_plastic);
	arrow.addInstance(glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 0.0125f, 0.025f, 0.15f, Material::green_plastic);
	arrow.addInstance(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f), 0.0125f, 0.025f, 0.15f, Material::cyan_plastic);
	//sphere.addInstance(glm::vec3(0.0f), glm::vec3(0.05f), Material::bronze);
	//surface.addInstance(glm::vec2(-10.f), glm::vec2(10.f), Material::yellow_plastic);
	//surface.addInstance(glm::vec2(-2.5f, -100.0f), glm::vec2(2.5f, -2.5f), Material::red_plastic);
	//surface.addInstance(glm::vec2(-2.5f, -100.0f), glm::vec2(-50.0f, 100.0f), Material::jade);

	// register programs
	programs.push_back(&arrow);
	//programs.push_back(&path);
	//programs.push_back(&sphere);
	//programs.push_back(&surface);
	//programs.push_back(&rect);
	programs.push_back(&stereo);
	programs.push_back(&grid);

	transitionPath->setCyclical();

	// setup programs
	for (Program* program : programs) {
		program->load();
	}

	// lighting
	DirLight dirLight = {
		glm::vec3(-0.2f, -0.9f, -0.2f),
		glm::vec4(0.5f, 0.5f, 0.5f, 1.0f),
		glm::vec4(0.75f, 0.75f, 0.75f, 1.0f),
		glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)
	};
	// write to UBO
	UBO::UBO dirLightUBO({
		UBO::newStruct({
			UBO::Type::VEC3,
			UBO::Type::VEC4,
			UBO::Type::VEC4,
			UBO::Type::VEC4
		})
	});
	for (Program* program : programs) {
		dirLightUBO.attachToShader(program->shader, "DirLightUniform");
	}
	// generate/bind
	dirLightUBO.generate();
	dirLightUBO.bind();
	dirLightUBO.initNullData(GL_STATIC_DRAW);
	dirLightUBO.bindRange();
	// write
	dirLightUBO.startWrite();
	dirLightUBO.writeElement<glm::vec3>(&dirLight.dir);
	dirLightUBO.writeElement<glm::vec4>(&dirLight.ambient);
	dirLightUBO.writeElement<glm::vec4>(&dirLight.diffuse);
	dirLightUBO.writeElement<glm::vec4>(&dirLight.specular);

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
		transitionPath->update(dt);
		for (Program* program : programs) {
			re_render |= program->update(dt);
		}

		// rendering
		if (re_render) {
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// render programs
			for (Program* program : programs) {
				program->render();
			}

			// move rendered buffer to screen
			glfwSwapBuffers(window);

			re_render = false;
		}
	}

	// cleanup programs
	for (Program* program : programs) {
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

	Keyboard::clearKeysChanged();
}

void updateCameraMatrices() {
	view = cam.getViewMatrix();
	projection = glm::perspective(
		glm::radians(cam.getZoom()),			// FOV
		(float)scr_width / (float)scr_height,	// aspect ratio
		0.1f, 100.0f							// near/far bounds
	);

	re_render = true;

	for (Program* program : programs) {
		program->updateCameraMatrices(projection * view, cam.cameraPos);
	}
}

void keyChanged(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (Keyboard::key(GLFW_KEY_ESCAPE)) {
		glfwSetWindowShouldClose(window, true);
	}

	if (key == GLFW_KEY_T && Keyboard::keyWentDown(key)) {
		transitionPath->toggleRunning();
	}

	for (Program* program : programs) {
		re_render |= program->keyChanged(window, key, scancode, action, mods);
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

	for (Program* program : programs) {
		re_render |= program->cursorChanged(window, _x, _y);
	}
}

void mouseButtonChanged(GLFWwindow* window, int button, int action, int mods) {
	for (Program* program : programs) {
		re_render |= program->mouseButtonChanged(window, button, action, mods);
	}
}

void scrollChanged(GLFWwindow* window, double dx, double dy) {
	// set camera zoom
	double scrollDy = Mouse::getScrollDY();
	if (scrollDy != 0) {
		cam.updateCameraZoom(scrollDy);
	}

	updateCameraMatrices();

	for (Program* program : programs) {
		re_render |= program->scrollChanged(window, dx, dy);
	}
}