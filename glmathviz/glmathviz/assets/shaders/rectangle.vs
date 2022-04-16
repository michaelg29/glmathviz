#version 330 core

layout (location = 0) in vec3 pos;

uniform mat4 projView;

void main() {
	// projection * view * model * vec4(pos, 1.0)
	gl_Position = projView * vec4(pos, 1.0);
}