#version 330 core
#define M_PI 3.1415926535897932384626433832795
#define M_PI_2 1.5707963267948966

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
uniform float t;

void sendVertex(vec3 pos, vec3 _fragColor) {
	fragColor = _fragColor;
	gl_Position = projView * vec4(pos, 1.0);
	EmitVertex();
}

float arg(float x, float y) {
	if (x == 0.0f) {
        if (y > 0.0f) {
            return M_PI_2;
        }
        if (y < 0.0f) {
            return -M_PI_2;
        }
        
        return 0.0;
    }
    if (x > 0.0f) {
        return atan(y / x);
    }
    if (x < 0.0f) {
        if (y >= 0.0f) {
            return atan(y / x) + M_PI;
        }

        return atan(y / x) - M_PI;
    }
}

// log(x + iy)
vec2 log_imag(float x, float y) {
	float mag = sqrt(x * x + y * y);
	return vec2(log(mag), arg(x, y));
}

// e^(x + iy)
vec2 exp_imag(float x, float y) {
	return exp(x) * vec2(cos(y), sin(y));
}

vec2 exp_imagvec(vec2 v) {
	return exp_imag(v.x, v.y);
}

// f(x + iy)
vec2 transform(float x, float y) {
	return exp_imagvec(3 * log_imag(x, y)); // (x + iy)^2
	// return log_imag(x, y);
	// return exp_imag(x, y);
}

void drawLine(vec2 start, vec2 end, int noPoints) {
	vec2 cur = start;
	vec2 inc = (end - start) / float(noPoints);
	for (int i = 0; i <= noPoints; i++) {
		float mag = length(cur) / float(max(x_cells, z_cells));
		vec2 transformed = transform(cur.x, cur.y);
		sendVertex(vec3(cur.x + t * (transformed.x - cur.x), 0.0, cur.y + t * (transformed.y - cur.y)), vec3(mag, mag, 1.0));
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