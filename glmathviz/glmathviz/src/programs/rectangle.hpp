#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include "program.h"
#include "../rendering/shader.h"
#include "../rendering/vertexmemory.hpp"

#ifndef RECTANGLE_HPP
#define RECTANGLE_HPP

class Rectangle : public Program {
private:
	ArrayObject VAO;
	glm::vec3 vertices[6] = {
		{ 0.5f, 0.5f, 0.0f },
		{ 0.5f, -0.5f, 0.0f },
		{ -0.5f, -0.5f, 0.0f },
		{ -0.5f, -0.5f, 0.0f },
		{ -0.5f, 0.5f, 0.0f },
		{ 0.5f, 0.5f, 0.0 }
	};

public:
	void load() {
		shader = Shader(false, "rectangle.vert", "rectangle.frag");

		VAO.generate();
		VAO.bind();
		VAO["VBO"] = BufferObject(GL_ARRAY_BUFFER);
		VAO["VBO"].generate();
		VAO["VBO"].bind();
		VAO["VBO"].setData<glm::vec3>(6, vertices, GL_STATIC_DRAW);
		VAO["VBO"].setAttPointer<GLfloat>(0, 3, GL_FLOAT, 3, 0);
	}

	void render() {
		shader.activate();
		VAO.bind();
		VAO.draw(GL_TRIANGLES, 0, 6);
	}

	void cleanup() {
		shader.cleanup();
		VAO.cleanup();
	}
};

#endif // RECTANGLE_HPP