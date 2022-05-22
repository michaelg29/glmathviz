#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include <vector>

#include "program.h"
#include "../rendering/shader.h"
#include "../rendering/material.h"
#include "../rendering/vertexmemory.hpp"

#ifndef SPHERE_HPP
#define SPHERE_HPP

typedef struct {
	glm::vec3 pos;
	glm::vec2 texCoord;
} SphereVertex;

class Sphere : public Program {
	std::vector<SphereVertex> vertices;
	std::vector<unsigned int> indices;

	unsigned int noInstances;
	unsigned int maxNoInstances;
	std::vector<glm::vec3> offsets;
	std::vector<glm::vec3> sizes;
	std::vector<glm::vec3> diffuse;
	std::vector<glm::vec4> specular;

	ArrayObject VAO;

	void addVertex(glm::vec3 pos, float phi, float th) {
		glm::vec2 texCoord;
		texCoord.x = th / glm::two_pi<float>();
		texCoord.y = (phi + glm::half_pi<float>()) / glm::pi<float>();

		vertices.push_back({ pos, texCoord });
	}

public:
	Sphere(unsigned int maxNoInstances)
		: maxNoInstances(maxNoInstances), noInstances(0) {}

	bool addInstance(glm::vec3 offset, glm::vec3 size, Material mat) {
		if (noInstances >= maxNoInstances) {
			return false;
		}

		offsets.push_back(offset);
		sizes.push_back(size);
		diffuse.push_back(mat.diffuse);
		specular.push_back(glm::vec4(mat.specular, mat.shininess));
		noInstances++;

		return true;
	}

	void load() {
		shader = Shader(false, "sphere.vert", "sphere.frag");

		// generate vertices
		unsigned int res = 100; // number of rows and columns
		float circleStep = glm::two_pi<float>() / (float)res; // angle step between cells
		float heightStep = glm::pi<float>() / (float)res; // height of row

		int row = 0;
		int noVertices = 0;
		float phi = -glm::half_pi<float>();
		float y = glm::sin(phi);
		float radius;

		for (; phi < glm::half_pi<float>() + heightStep; phi += heightStep, row++) {
			y = glm::sin(phi);
			radius = glm::cos(phi);
			int cell = 0;
			for (float th = 0; th < glm::two_pi<float>(); th += circleStep, cell++) {
				addVertex(
					glm::vec3(radius * glm::cos(th), y, radius * glm::sin(th)),
					phi, th
				);

				// add indices if not bottom row
				if (row)
				{
					int nextCell = (cell + 1) % res;
					indices.push_back(noVertices - res); // bottom left
					indices.push_back((row - 1) * res + nextCell); // bottom right
					indices.push_back(row * res + nextCell); // top right

					indices.push_back(noVertices - res); // bottom left
					indices.push_back(noVertices); // top left (this vertex)
					indices.push_back(row * res + nextCell); // top right
				}

				noVertices++;
			}
		}

		// setup VAO
		VAO.generate();
		VAO.bind();

		// load data
		VAO["VBO"] = BufferObject(GL_ARRAY_BUFFER);
		VAO["VBO"].generate();
		VAO["VBO"].bind();
		VAO["VBO"].setData<SphereVertex>((GLuint)vertices.size(), &vertices[0], GL_STATIC_DRAW);
		VAO["VBO"].setAttPointer<GLfloat>(0, 3, GL_FLOAT, 5, 0); // pos
		VAO["VBO"].setAttPointer<GLfloat>(1, 2, GL_FLOAT, 5, 3); // tex coord

		VAO["EBO"] = BufferObject(GL_ELEMENT_ARRAY_BUFFER);
		VAO["EBO"].generate();
		VAO["EBO"].bind();
		VAO["EBO"].setData<GLuint>((GLuint)indices.size(), &indices[0], GL_STATIC_DRAW);
	
		if (noInstances) {
			VAO["offsetVBO"] = BufferObject(GL_ARRAY_BUFFER);
			VAO["offsetVBO"].generate();
			VAO["offsetVBO"].bind();
			VAO["offsetVBO"].setData<glm::vec3>(noInstances, &offsets[0], GL_STATIC_DRAW);
			VAO["offsetVBO"].setAttPointer<GLfloat>(2, 3, GL_FLOAT, 3, 0, 1);

			VAO["sizeVBO"] = BufferObject(GL_ARRAY_BUFFER);
			VAO["sizeVBO"].generate();
			VAO["sizeVBO"].bind();
			VAO["sizeVBO"].setData<glm::vec3>(noInstances, &sizes[0], GL_STATIC_DRAW);
			VAO["sizeVBO"].setAttPointer<GLfloat>(3, 3, GL_FLOAT, 3, 0, 1);

			VAO["diffuseVBO"] = BufferObject(GL_ARRAY_BUFFER);
			VAO["diffuseVBO"].generate();
			VAO["diffuseVBO"].bind();
			VAO["diffuseVBO"].setData<glm::vec3>(noInstances, &diffuse[0], GL_STATIC_DRAW);
			VAO["diffuseVBO"].setAttPointer<GLfloat>(4, 3, GL_FLOAT, 3, 0, 1);

			VAO["specularVBO"] = BufferObject(GL_ARRAY_BUFFER);
			VAO["specularVBO"].generate();
			VAO["specularVBO"].bind();
			VAO["specularVBO"].setData<glm::vec4>(noInstances, &specular[0], GL_STATIC_DRAW);
			VAO["specularVBO"].setAttPointer<GLfloat>(5, 4, GL_FLOAT, 4, 0, 1);
		}
	}

	void render() {
		shader.activate();
		VAO.bind();
		VAO.draw(GL_TRIANGLES, (GLuint)indices.size(), GL_UNSIGNED_INT, 0, noInstances);
	}

	void cleanup() {
		shader.cleanup();
		VAO.cleanup();

		offsets.clear();
		sizes.clear();
		diffuse.clear();
		specular.clear();
	}
};

#endif