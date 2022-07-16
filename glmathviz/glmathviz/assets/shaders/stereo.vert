#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec3 offset;
layout (location = 3) in vec3 size;
layout (location = 4) in vec3 diffuse;
layout (location = 5) in vec4 specular; // vec3 specular, float shininess

out vec2 tex;
out vec3 fragPos;
out vec3 normal;
out vec3 diffMap;
out vec3 specMap;
out float shininess;

uniform mat4 projView;
uniform float t;

void main() {
	tex = texCoord;

	float factor = 1.0 + t * pos.y / (1.0 - pos.y);
	float x = pos.x * factor;
	float y = (1.0 - t) * pos.y;
	float z = pos.z * factor;
	fragPos = vec3(x, y, z);
	normal = pos;
	diffMap = (pos + vec3(1.0)) / 2.0;
	specMap = specular.xyz;
	shininess = specular.w;

	gl_Position = projView * vec4(fragPos, 1.0);
}
