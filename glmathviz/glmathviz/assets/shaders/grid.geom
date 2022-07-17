#version 330 core

// take in points
layout (points) in;
// out line segments with an adjacent vertex
layout (line_strip, max_vertices=16) out;

in VS_OUT {
	int idx;
} gs_in[];
out vec3 fragColor;

uniform mat4 projView;
uniform vec2 minBound;
uniform vec2 maxBound;
uniform int x_cells;
uniform int z_cells;

void sendVertex(vec3 pos) {
	fragColor = vec3(0.5, 0.5, 1.0);
	gl_Position = projView * vec4(pos, 1.0);
	EmitVertex();
}

void drawLine(vec2 start, vec2 end, int noPoints) {
	vec2 cur = start;
	vec2 inc = (end - start) / float(noPoints);
	for (int i = 0; i <= noPoints; i++) {
		sendVertex(vec3(cur.x, 0.0, cur.y));
		cur += inc;
	}
}

void main() {
	// calculate increment
	float x_inc = (maxBound.x - minBound.x) / float(x_cells);
	float z_inc = (maxBound.y - minBound.y) / float(z_cells);

	// calculate origin point using index
	float x = float(gs_in[0].idx / z_cells) * x_inc + minBound.x;
	float z = float(gs_in[0].idx % z_cells) * z_inc + minBound.y;

	drawLine(vec2(x - 0.5, z), vec2(x + 0.5, z), 7);
	EndPrimitive();
	drawLine(vec2(x, z - 0.5), vec2(x, z + 0.5), 7);
	EndPrimitive();
}