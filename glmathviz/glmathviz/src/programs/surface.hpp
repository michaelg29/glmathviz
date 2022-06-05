#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <vector>

#include "program.h"
#include "../rendering/shader.h"
#include "../rendering/material.h"
#include "../rendering/vertexmemory.hpp"
#include "../io/keyboard.h"

#ifndef SURFACE_HPP
#define SURFACE_HPP

class Surface : public Program {
	ArrayObject VAO;
	int x_cells;
	int z_cells;

	bool calculus;

	unsigned int noInstances;
	unsigned int maxNoInstances;
	std::vector<glm::vec4> bounds;
	std::vector<glm::vec3> diffuse;
	std::vector<glm::vec4> specular;

public:
	Surface(unsigned int maxNoInstances, int x_cells, int z_cells)
		: noInstances(0), maxNoInstances(maxNoInstances), 
		x_cells(x_cells), z_cells(z_cells),
		calculus(true) {}

	bool addInstance(glm::vec2 start, glm::vec2 end, Material material) {
		if (noInstances >= maxNoInstances) {
			return false;
		}

		bounds.push_back(glm::vec4(start, end));
		diffuse.push_back(material.diffuse);
		specular.push_back(glm::vec4(material.specular, material.shininess));

		noInstances++;
		return true;
	}

	void load() {
		shader = Shader(false, "surface.vert", "dirlight.frag", "surface.geom");
		shader.activate();
		shader.setInt("x_cells", x_cells);
		shader.setInt("z_cells", z_cells);
		shader.setBool("calculus", calculus);

		VAO.generate();
		VAO.bind();

		if (noInstances) {
			VAO["boundsVBO"] = BufferObject(GL_ARRAY_BUFFER);
			VAO["boundsVBO"].generate();
			VAO["boundsVBO"].bind();
			VAO["boundsVBO"].setData<glm::vec4>(noInstances, &bounds[0], GL_STATIC_DRAW);
			VAO["boundsVBO"].setAttPointer<GLfloat>(0, 4, GL_FLOAT, 4, 0, 1);

			VAO["diffuseVBO"] = BufferObject(GL_ARRAY_BUFFER);
			VAO["diffuseVBO"].generate();
			VAO["diffuseVBO"].bind();
			VAO["diffuseVBO"].setData<glm::vec3>(noInstances, &diffuse[0], GL_STATIC_DRAW);
			VAO["diffuseVBO"].setAttPointer<GLfloat>(1, 3, GL_FLOAT, 3, 0, 1);

			VAO["specularVBO"] = BufferObject(GL_ARRAY_BUFFER);
			VAO["specularVBO"].generate();
			VAO["specularVBO"].bind();
			VAO["specularVBO"].setData<glm::vec4>(noInstances, &specular[0], GL_STATIC_DRAW);
			VAO["specularVBO"].setAttPointer<GLfloat>(2, 4, GL_FLOAT, 4, 0, 1);
		}
	}

	void render() {
		shader.activate();
		VAO.bind();
		VAO.draw(GL_POINTS, 0, x_cells * z_cells, noInstances);
	}

	void cleanup() {
		shader.cleanup();
		VAO.cleanup();
		bounds.clear();
		diffuse.clear();
		specular.clear();
	}

	bool keyChanged(GLFWwindow* window, int key, int scancode, int action, int mods) {
		if (key == GLFW_KEY_C) {
			if (Keyboard::keyWentDown(GLFW_KEY_C)) {
				calculus = !calculus;
				shader.setBool("calculus", calculus);
				return true;
			}
		}

		return false;
	}
};

#endif // SURFACE_HPP