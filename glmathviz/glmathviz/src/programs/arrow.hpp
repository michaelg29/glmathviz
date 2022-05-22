#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include <vector>

#include "program.h"
#include "../rendering/shader.h"
#include "../rendering/material.h"
#include "../rendering/vertexmemory.hpp"

#ifndef ARROW_HPP
#define ARROW_HPP

class Arrow : public Program {
	unsigned int noInstances;
	unsigned int maxNoInstances;

	std::vector<glm::vec4> dimensions; // magnitude, arm_radius, head_radius, head_height
	std::vector<glm::mat4> modelMats;
	std::vector<glm::mat3> normalModelMats;
	std::vector<glm::vec3> diffuse;
	std::vector<glm::vec4> specular; // specular, shininess

	ArrayObject VAO;

public:
	Arrow(unsigned int maxNoInstances)
		: maxNoInstances(maxNoInstances), noInstances(0) {}

	bool addInstance(glm::vec3 start, glm::vec3 end, float armRadius, float headRadius, float headHeight, Material material) {
		if (noInstances >= maxNoInstances) {
			return false;
		}

		// calculate model matrix
		glm::mat4 mat(1.0f);
		glm::vec3 armVector = glm::normalize(end - start);

		glm::vec3 u, v;
		if (armVector.y != 0.0f) {
			u = glm::vec3(1.0f, -armVector.x / armVector.y, 0.0f);
			v = glm::vec3(0.0f, -armVector.z / armVector.y, 1.0f);
		}
		else if (armVector.z != 0.0f) {
			u = glm::vec3(1.0f, 0.0f, -armVector.x / armVector.z);
			v = glm::vec3(0.0f, 1.0f, -armVector.y / armVector.z);
		}
		else if (armVector.x != 0.0f) {
			u = glm::vec3(-armVector.y / armVector.x, 1.0f, 0.0f);
			v = glm::vec3(-armVector.z / armVector.x, 0.0f, 1.0f);
		}
		else {
			// two points are the same
			return false;
		}

		mat[0] = glm::vec4(glm::normalize(u), 0.0f);
		mat[1] = glm::vec4(armVector, 0.0f);
		mat[2] = glm::vec4(glm::normalize(v), 0.0f);
		mat[3] = glm::vec4(start, 1.0f);
		modelMats.push_back(mat);
		normalModelMats.push_back(glm::transpose(glm::inverse(glm::mat3(mat))));

		dimensions.push_back({ glm::length(end - start), armRadius, headRadius, headHeight });
		diffuse.push_back(material.diffuse);
		specular.push_back(glm::vec4(material.specular, material.shininess));

		noInstances++;

		return true;
	}

	void load() {
		shader = Shader(false, "arrow.vs", "sphere.fs", "arrow.gs");

		if (!noInstances) {
			return;
		}

		VAO.generate();
		VAO.bind();

		VAO["dimVBO"] = BufferObject(GL_ARRAY_BUFFER);
		VAO["dimVBO"].generate();
		VAO["dimVBO"].bind();
		VAO["dimVBO"].setData<glm::vec4>(noInstances, &dimensions[0], GL_STATIC_DRAW);
		VAO["dimVBO"].setAttPointer<GLfloat>(0, 4, GL_FLOAT, 4, 0);

		VAO["matVBO"] = BufferObject(GL_ARRAY_BUFFER);
		VAO["matVBO"].generate();
		VAO["matVBO"].bind();
		VAO["matVBO"].setData<glm::mat4>(noInstances, &modelMats[0], GL_STATIC_DRAW);
		VAO["matVBO"].setAttPointer<glm::vec4>(1, 4, GL_FLOAT, 4, 0);
		VAO["matVBO"].setAttPointer<glm::vec4>(2, 4, GL_FLOAT, 4, 1);
		VAO["matVBO"].setAttPointer<glm::vec4>(3, 4, GL_FLOAT, 4, 2);
		VAO["matVBO"].setAttPointer<glm::vec4>(4, 4, GL_FLOAT, 4, 3);

		VAO["normVBO"] = BufferObject(GL_ARRAY_BUFFER);
		VAO["normVBO"].generate();
		VAO["normVBO"].bind();
		VAO["normVBO"].setData<glm::mat3>(noInstances, &normalModelMats[0], GL_STATIC_DRAW);
		VAO["normVBO"].setAttPointer<glm::vec3>(5, 3, GL_FLOAT, 3, 0);
		VAO["normVBO"].setAttPointer<glm::vec3>(6, 3, GL_FLOAT, 3, 1);
		VAO["normVBO"].setAttPointer<glm::vec3>(7, 3, GL_FLOAT, 3, 2);

		VAO["diffVBO"] = BufferObject(GL_ARRAY_BUFFER);
		VAO["diffVBO"].generate();
		VAO["diffVBO"].bind();
		VAO["diffVBO"].setData<glm::vec3>(noInstances, &diffuse[0], GL_STATIC_DRAW);
		VAO["diffVBO"].setAttPointer<GLfloat>(8, 3, GL_FLOAT, 3, 0);

		VAO["specVBO"] = BufferObject(GL_ARRAY_BUFFER);
		VAO["specVBO"].generate();
		VAO["specVBO"].bind();
		VAO["specVBO"].setData<glm::vec4>(noInstances, &specular[0], GL_STATIC_DRAW);
		VAO["specVBO"].setAttPointer<GLfloat>(9, 4, GL_FLOAT, 4, 0);
	}

	void render() {
		shader.activate();
		VAO.bind();
		VAO.draw(GL_POINTS, 0, noInstances);
	}

	void cleanup() {
		noInstances = 0;

		dimensions.clear();
		modelMats.clear();
		normalModelMats.clear();
		diffuse.clear();
		specular.clear();

		shader.cleanup();
		VAO.cleanup();
	}
};

#endif // ARROW_HPP