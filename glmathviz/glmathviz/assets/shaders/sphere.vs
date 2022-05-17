#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec3 offset;
layout (location = 3) in vec3 size;

out vec2 tex;
out vec3 fragPos;
out vec3 normal;

uniform mat4 projView;

void main() {
	tex = texCoord;
	fragPos = size * pos + offset;
	normal = pos;
	gl_Position = projView * vec4(fragPos, 1.0);
}
