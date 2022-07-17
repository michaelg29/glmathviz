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

public:
	Grid(glm::ivec2 min, glm::ivec2 max)
		: min((float)min.x, (float)min.y),
		max((float)max.x, (float)max.y),
		x_cells(max.x - min.x),
		z_cells(max.y - min.y) { }

	void load() {
		shader = Shader(false, "grid.vert", "color.frag", "grid.geom");
		shader.activate();
		shader.set2Float("minBound", min);
		shader.set2Float("maxBound", max);
		shader.setInt("x_cells", x_cells);
		shader.setInt("z_cells", z_cells);

		VAO.generate();
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
};

#endif // GRID_HPP