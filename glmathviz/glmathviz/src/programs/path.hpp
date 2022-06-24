#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include <vector>

#include "program.h"
#include "../io/keyboard.h"
#include "../rendering/shader.h"
#include "../rendering/vertexmemory.hpp"
#include "../rendering/transition.hpp"

#ifndef PATH_HPP
#define PATH_HPP

class Path : public Program {
	std::vector<glm::vec3> points;

	ArrayObject VAO;

	Transition<glm::vec3> *path;
	unsigned int maxPoints;
	double stopwatch;
	double stopwatchIncrement;

public:
	Path(Transition<glm::vec3> *path, unsigned int resolution = 100)
		: path(path),
		maxPoints(resolution + 1), // +1 because resolution is for line segments
		stopwatchIncrement(path->getDuration() / (double)resolution)
	{}

	void load() {
		shader = Shader(false, "rectangle.vert", "rectangle.frag");

		VAO.generate();
		VAO.bind();

		points.push_back(path->getCurrent());

		VAO["VBO"] = BufferObject(GL_ARRAY_BUFFER);
		VAO["VBO"].generate();
		VAO["VBO"].bind();
		VAO["VBO"].setData<glm::vec3>(maxPoints, NULL, GL_DYNAMIC_DRAW);
		VAO["VBO"].setAttPointer<GLfloat>(0, 3, GL_FLOAT, 3, 0);
	}

	bool update(double dt) {
		if (path->isRunning() && points.size() < maxPoints) {
			stopwatch += dt;
			if (stopwatch >= stopwatchIncrement) {
				points.push_back(path->getCurrent());
				VAO.bind();
				VAO["VBO"].bind();
				VAO["VBO"].updateData<glm::vec3>(0, (GLuint)points.size(), &points[0]);

				stopwatch = 0.0;
				return true;
			}
		}

		return false;
	}

	void render() {
		shader.activate();
		VAO.bind();
		VAO.draw(GL_LINE_STRIP, 0, (GLuint)points.size());
	}

	void cleanup() {
		VAO.cleanup();
		points.clear();
		shader.cleanup();
	}
};

#endif