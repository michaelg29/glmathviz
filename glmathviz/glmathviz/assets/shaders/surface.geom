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

// surface y = f(x, z)
float func(float x, float z) {
	float denom = x*x + z*z;

	return denom <= 0.01 ? 300.0 : 1 / denom;
}

// normal vector to surface y = f(x, z)
vec3 funcNorm(vec3 p) {
	return vec3(2 * p.x * p.y * p.y, 1, 2 * p.z * p.y * p.y);
}

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