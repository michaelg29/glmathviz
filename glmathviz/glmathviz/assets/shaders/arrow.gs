#version 330 core

#define M_PI 3.14159265358979323846

// take in points
layout (points) in;
// output triangles
layout (triangle_strip, max_vertices = 56) out;

in VS_OUT {
	float mag;
	float armRadius;
	float headRadius;
	float headHeight;

	mat4 model;
	mat3 normalModel;

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

int noEdges = 15;

void sendVertex(vec3 pos, vec3 norm) {
	vec4 transformedPos = gs_in[0].model * vec4(pos, 1.0);
	fragPos = transformedPos.xyz;
	normal = gs_in[0].normalModel * norm;
	gl_Position = projView * transformedPos;
	EmitVertex();
}

void buildCylinder(float height, float radius) {
	float increment = 2 * M_PI / float(noEdges);

	for (float i = 0.0; i < float(noEdges) + 1.0; i += 1.0) {
		vec3 curXZ = vec3(radius * cos(i * increment), 0.0, radius * sin(i * increment));
		vec3 norm = curXZ;

		sendVertex(curXZ, norm);
		sendVertex(vec3(curXZ.x, height, curXZ.z), norm);
	}
	EndPrimitive();
}

void buildCone(float headRadius, float mag, float headHeight) {
	float increment = 2 * M_PI / float(noEdges);

	bool sendHead = true;
	for (float i = 0.0; i < float(noEdges) + 1.0; i += 1.0) {
		vec3 curXZ = vec3(headRadius * cos(i * increment), mag - headHeight, headRadius * sin(i * increment));
		vec3 norm = vec3(cos(i * increment), -1.0 / (-headHeight / headRadius), sin(i * increment));

		sendVertex(curXZ, norm);
		if (sendHead) {
			sendVertex(vec3(0.0, mag, 0.0), norm);
		}
		sendHead = !sendHead;
	}
	EndPrimitive();
}

void main() {
	tex = vec2(0.0);
	diffMap = gs_in[0].diffuse;
	specMap = gs_in[0].specular;
	shininess = gs_in[0].shininess;

	buildCylinder(gs_in[0].mag - gs_in[0].headHeight, gs_in[0].armRadius);
	buildCone(gs_in[0].headRadius, gs_in[0].mag, gs_in[0].headHeight);
}
