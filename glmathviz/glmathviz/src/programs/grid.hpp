#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <vector>

#include "program.h"
#include "../rendering/shader.h"
#include "../rendering/material.h"
#include "../rendering/vertexmemory.hpp"
#include "../rendering/transition.hpp"
#include "../io/keyboard.h"

#ifndef GRID_HPP
#define GRID_HPP

class Grid : public Program {
	ArrayObject VAO;
	glm::vec2 min;
	glm::vec2 max;
	int x_cells;
	int z_cells;

	double direction = 1.0;
	CubicBezierTransition<double> transition;

public:
	Grid(glm::ivec2 min, glm::ivec2 max)
		: min((float)min.x, (float)min.y),
		max((float)max.x, (float)max.y),
		x_cells(max.x - min.x),
		z_cells(max.y - min.y),
		transition(CubicBezierTransition<double>::newEaseTransition(0.0, 1.0, 5.0)) { }

	void load() {
		shader = Shader(false, "grid.vert", "color.frag", "grid.geom");
		shader.activate();
		shader.set2Float("minBound", min);
		shader.set2Float("maxBound", max);
		shader.setInt("x_cells", x_cells);
		shader.setInt("z_cells", z_cells);
		shader.setFloat("t", 0.0f);

		VAO.generate();
	}

	bool update(double dt) {
		if (transition.isRunning()) {
			transition.update(direction * dt);
			shader.activate();
			shader.setFloat("t", (float)transition.getCurrent());
			return true;
		}

		return false;
	}

	void render() {
		shader.activate();
		VAO.bind();
		VAO.draw(GL_POINTS, 0, x_cells * z_cells);
	}

	void cleanup() {
		shader.cleanup();
		VAO.cleanup();
	}

	bool keyChanged(GLFWwindow* window, int key, int scancode, int action, int mods) {
		if (key == GLFW_KEY_LEFT_BRACKET && Keyboard::keyWentDown(GLFW_KEY_LEFT_BRACKET)) {
			direction = -1.0;
			transition.run();
		}
		if (key == GLFW_KEY_RIGHT_BRACKET && Keyboard::keyWentDown(GLFW_KEY_RIGHT_BRACKET)) {
			direction = 1.0;
			transition.run();
		}
		if (key == GLFW_KEY_BACKSLASH && Keyboard::keyWentDown(GLFW_KEY_BACKSLASH)) {
			transition.toggleRunning();
		}

		return false;
	}
};

#endif // GRID_HPP