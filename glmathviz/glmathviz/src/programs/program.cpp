#include "program.h"

void Program::updateCameraMatrices(glm::mat4 projView, glm::vec3 camPos) {
	shader.activate();
	shader.setMat4("projView", projView);
	shader.set3Float("viewPos", camPos);
}

void Program::load() {}
void Program::render(double dt) {}
void Program::cleanup() {}