#version 330 core

// take in points
layout (points) in;
// out two triangles with an adjacent side
layout (triangle_strip, max_vertices=4) out;

in VS_OUT {
	int idx;

	vec2 minBound;
	vec2 maxBound;

	vec3 diffuse;
	vec3 specular;
	float shininess;
} gs_in[];
out vec2 tex;
out vec3 fragPos;
out vec3 normal;
out vec3 diffMap;
out vec3 specMap;
out float shininess;

uniform mat4 projView;
uniform int x_cells;
uniform int z_cells;
uniform bool calculus;

void sendVertex(vec3 pos, vec3 norm) {
	fragPos = pos;
	normal = norm;
	gl_Position = projView * vec4(pos, 1.0);
	EmitVertex();
}

uniform float x_offset;

// ===================================================================
// CUSTOMIZE THIS TO AFFECT THE OUTPUT
// surface y = f(x, z)
float func(float x, float z) {
	x -= x_offset;
	return 1 / (x*x + z*z); // f1
	return 1 - abs(x + z) - abs(z - x); // f6
	return (sign(-0.65 - x) + sign(-0.35 - x) + sign(-0.5 - x) + sign(0.25 - x) + sign(0.55 - x)) / 7; // f5
	return floor(exp(abs(x*z/2.0)) + round(1/cos(x*z))); // f4, can only be visualized with cross product
	float f3 = (x*x + z*z) / 4; // f3
	return 3 * sin(f3) / f3;
	return cos(x) * sin(z); // f2
	return 1 / (x*x + z*z); // f1
}

// normal vector to surface y = f(x, z)
vec3 funcNorm(vec3 p) {
	p.x -= x_offset;
	return vec3(2 * p.x * p.y * p.y, 1, 2 * p.z * p.y * p.y); // f1
	return vec3(0.0); // f6
	return vec3(0.0); // f5
	return vec3(0.0); // f4
	float f3 = (p.x*p.x + p.z*p.z) / 4; // f3
	return 3 * vec3(p.x * (sin(f3) - f3 * cos(f3)) / 2 / f3 / f3, 1, p.z * (sin(f3) - f3 * cos(f3)) / 2 / f3 / f3);
	return vec3(sin(p.x) * sin(p.z), 1, -cos(p.x) * cos(p.z)); // f2
	return vec3(2 * p.x * p.y * p.y, 1, 2 * p.z * p.y * p.y); // f1
}
// ===================================================================

void normalCalculus(float x, float z, float x_inc, float z_inc) {
	// calculate corner points
	vec3 p00 = vec3(x, func(x, z), z);
	vec3 p01 = vec3(x, func(x, z + z_inc), z + z_inc);
	vec3 p10 = vec3(x + x_inc, func(x + x_inc, z), z);
	vec3 p11 = vec3(x + x_inc, func(x + x_inc, z + z_inc), z + z_inc);

	// normals calculated using custom function
	sendVertex(p00, funcNorm(p00));
	sendVertex(p01, funcNorm(p01));
	sendVertex(p10, funcNorm(p10));
	sendVertex(p11, funcNorm(p11));
}

void normalCrossProd(float x, float z, float x_inc, float z_inc) {
	// calculate corner points
	vec3 p00 = vec3(x, func(x, z), z);
	vec3 p01 = vec3(x, func(x, z + z_inc), z + z_inc);
	vec3 p10 = vec3(x + x_inc, func(x + x_inc, z), z);
	vec3 p11 = vec3(x + x_inc, func(x + x_inc, z + z_inc), z + z_inc);

	// normals are cross product of vectors in the surface
	sendVertex(p00, cross(p01 - p00, p10 - p00));
	sendVertex(p01, cross(p11 - p01, p00 - p01));
	sendVertex(p10, cross(p00 - p10, p11 - p10));
	sendVertex(p11, cross(p10 - p11, p01 - p11));
}

void main() {
	diffMap = gs_in[0].diffuse;
	specMap = gs_in[0].specular;
	shininess = gs_in[0].shininess;

	// calculate increment
	float x_inc = (gs_in[0].maxBound.x - gs_in[0].minBound.x) / float(x_cells);
	float z_inc = (gs_in[0].maxBound.y - gs_in[0].minBound.y) / float(z_cells);

	// calculate origin point using index
	float x = float(gs_in[0].idx / z_cells) * x_inc + gs_in[0].minBound.x;
	float z = float(gs_in[0].idx % z_cells) * z_inc + gs_in[0].minBound.y;

	if (calculus) {
		// calculus to calculate normal vector
		normalCalculus(x, z, x_inc, z_inc);
	}
	else {
		// cross product to calculate normal vector
		normalCrossProd(x, z, x_inc, z_inc);
	}

	EndPrimitive();
}