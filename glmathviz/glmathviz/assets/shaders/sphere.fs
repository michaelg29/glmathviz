#version 330 core

in vec2 tex;
in vec3 fragPos;

out vec4 fragColor;

void main() {
	fragColor = vec4(tex, 0.0, 1.0);
}
