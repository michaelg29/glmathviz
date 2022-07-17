#version 330 core

out VS_OUT {
	int idx;
} vs_out;

void main() {
	vs_out.idx = gl_VertexID;
}