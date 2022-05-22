#include "program.h"

void Program::updateCameraMatrices(glm::mat4 projView, glm::vec3 camPos) {
	shader.activate();
	shader.setMat4("projView", projView);
	shader.set3Float("viewPos", camPos);
}

void Program::load() {}
bool Program::update(double dt) { return false; }
void Program::render() {}
void Program::cleanup() {}

void Program::processInput(double dt, GLFWwindow* window) {}
void Program::keyChanged(GLFWwindow* window, int key, int scancode, int action, int mods) {}
void Program::cursorChanged(GLFWwindow* window, double _x, double _y) {}
void Program::mouseButtonChanged(GLFWwindow* window, int button, int action, int mods) {}
void Program::scrollChanged(GLFWwindow* window, double dx, double dy) {}