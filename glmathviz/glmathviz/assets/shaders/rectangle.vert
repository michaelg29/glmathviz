#version 330 core

layout (location = 0) in vec3 pos;

out vec3 fragPos;

uniform mat4 projView;

void main() {
	// projection * view * model * vec4(pos, 1.0)
	fragPos = pos;
	gl_Position = projView * vec4(pos, 1.0);
}