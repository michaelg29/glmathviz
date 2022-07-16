#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include <vector>

#include "program.h"
#include "sphere.hpp"
#include "../io/keyboard.h"
#include "../rendering/shader.h"
#include "../rendering/material.h"
#include "../rendering/vertexmemory.hpp"
#include "../rendering/transition.hpp"

#ifndef STEREOGRAPHIC_PROJ_HPP
#define STEREOGRAPHIC_PROJ_HPP

class StereographicProj : public Sphere {
	double direction = 1.0;
	CubicBezierTransition<double> transition;

public:
	StereographicProj()
		: Sphere(nullptr, 1),
		transition(CubicBezierTransition<double>::newEaseTransition(0.0, 1.0, 5.0)) { }

	void load() {
		addInstance(glm::vec3(0.0f), glm::vec3(1.0f), Material::yellow_plastic);
		Sphere::load();

		shader.cleanup();
		shader = Shader(false, "stereo.vert", "dirlight.frag");
		shader.setFloat("t", 0.0f);
	}

	bool update(double dt) {
		if (transition.isRunning()) {
			transition.update(direction * dt);
			shader.setFloat("t", (float)transition.getCurrent());
			return true;
		}

		return false;
	}

	void render() {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		Sphere::render();
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	bool keyChanged(GLFWwindow* window, int key, int scancode, int action, int mods) {
		if (key == GLFW_KEY_RIGHT && Keyboard::keyWentDown(GLFW_KEY_RIGHT)) {
			direction = 1.0;
			transition.run();
		}
		if (key == GLFW_KEY_LEFT && Keyboard::keyWentDown(GLFW_KEY_LEFT)) {
			direction = -1.0;
			transition.run();
		}
		if (key == GLFW_KEY_DOWN && Keyboard::keyWentDown(GLFW_KEY_DOWN)) {
			transition.toggleRunning();
		}

		return false;
	}
};

#endif // STEREOGRAPHIC_PROJ_HPP