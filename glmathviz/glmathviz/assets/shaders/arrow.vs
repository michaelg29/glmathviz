#version 330 core

layout (location = 0) in vec4 dimensions;
layout (location = 1) in mat4 modelMat;
layout (location = 5) in mat3 normModelMat;
layout (location = 8) in vec3 diffuse;
layout (location = 9) in vec4 specular;

out VS_OUT {
	float mag;
	float armRadius;
	float headRadius;
	float headHeight;

	mat4 model;
	mat3 normalModel;

	vec3 diffuse;
	vec3 specular;
	float shininess;
} vs_out;

void main() {
	vs_out.mag = dimensions.x;
	vs_out.armRadius = dimensions.y;
	vs_out.headRadius = dimensions.z;
	vs_out.headHeight = dimensions.w;

	vs_out.model = modelMat;
	vs_out.normalModel = normModelMat;

	vs_out.diffuse = diffuse;
	vs_out.specular = specular.xyz;
	vs_out.shininess = specular.w;
}
