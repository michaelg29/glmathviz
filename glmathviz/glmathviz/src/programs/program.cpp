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