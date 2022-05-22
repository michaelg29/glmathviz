#version 330 core

in vec2 tex;
in vec3 fragPos;
in vec3 normal;
in vec3 diffMap;
in vec3 specMap;
in float shininess;

uniform vec3 viewPos;

out vec4 fragColor;

struct DirLight {
	vec3 direction;

	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
};

layout (std140) uniform DirLightUniform {
	DirLight dirLight;
};

vec4 calcDirLight(DirLight dirLight, vec3 norm, vec3 viewVec, vec4 diffMap, vec4 specMap);

void main() {
	fragColor = vec4(0.0, 0.0, 0.0, 1.0);

	fragColor += calcDirLight(dirLight, normalize(normal), normalize(viewPos - fragPos), vec4(diffMap, 1.0), vec4(specMap, 1.0));
}

vec4 calcDirLight(DirLight dirLight, vec3 norm, vec3 viewDir, vec4 diffMap, vec4 specMap) {
	// ambient
	vec4 ambient = dirLight.ambient * diffMap;

	// diffuse
	vec3 lightDir = normalize(-dirLight.direction);
	float diff = max(dot(norm, lightDir), 0.0);
	vec4 diffuse = dirLight.diffuse * (diff * diffMap);

	// specular
	vec4 specular = vec4(0.0, 0.0, 0.0, 1.0);
	if (diff > 0) {
		vec3 halfwayDir = normalize(lightDir + viewDir);
		float dotProd = dot(norm, halfwayDir);

		float spec = pow(max(dotProd, 0.0), shininess * 128);
		specular = dirLight.specular * (spec * specMap);
	}

	return vec4(ambient + diffuse + specular);
}