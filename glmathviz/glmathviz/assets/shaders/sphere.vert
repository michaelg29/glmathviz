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

void main() {
	tex = texCoord;
	fragPos = size * pos + offset;
	normal = pos;
	diffMap = diffuse;
	specMap = specular.xyz;
	shininess = specular.w;

	gl_Position = projView * vec4(fragPos, 1.0);
}
