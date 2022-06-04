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

void sendVertex(vec3 pos, vec3 norm) {
	fragPos = pos;
	normal = norm;
	gl_Position = projView * vec4(pos, 1.0);
	EmitVertex();
}

vec4 funcAndNorm(float x, float z) {
	// r(x, z) = (x, f(x, z), z)
	// customizable for f
	float denom = x*x + z*z;
	float y = 0.0;
	if (abs(denom) <= 0.01) {
		y = 300.0;
	}
	else {
		y = 1 / denom;
	}

	vec3 norm = vec3(2 * x * y * y, 1, 2 * z * y * y); // cross(dr/dz, dr/dx)

	return vec4(y, norm);
}

//vec4 funcAndNorm(float x, float z) {
//	float y = cos(x) + cos(z);
//	vec3 norm = vec3(sin(x), 1, sin(z));
//
//	return vec4(y, norm);
//}

void calcAndSendPoint(float x, float z) {
	vec4 yNorm = funcAndNorm(x, z);
	fragPos = vec3(x, yNorm.x, z);
	normal = yNorm.yzw;
	gl_Position = projView * vec4(fragPos, 1.0);
	EmitVertex();
}

vec3 func(float x, float z) {
	return vec3(x, cos(x) + cos(z), z);
}

void calcPoints(float x, float z, float x_inc, float z_inc) {
	vec3 p00 = func(x, z);
	vec3 p01 = func(x, z + z_inc);
	vec3 p10 = func(x + x_inc, z);
	vec3 p11 = func(x + x_inc, z + z_inc);

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

	// calculus to calculate normal vector
	calcAndSendPoint(x, z);
	calcAndSendPoint(x, z + z_inc);
	calcAndSendPoint(x + x_inc, z);
	calcAndSendPoint(x + x_inc, z + z_inc);

	// cross product to calculate normal vector
	//calcPoints(x, z, x_inc, z_inc);

	EndPrimitive();
}