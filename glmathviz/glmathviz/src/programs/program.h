#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../rendering/shader.h"

#ifndef PROGRAM_H
#define PROGRAM_H

class Program {
public:
	Shader shader;

	void updateCameraMatrices(glm::mat4 projView, glm::vec3 camPos);
	virtual void load();
	virtual void render(double dt);
	virtual void cleanup();
};

#endif // PROGRAM_H