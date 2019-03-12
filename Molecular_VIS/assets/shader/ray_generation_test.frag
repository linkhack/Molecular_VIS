#version 430 core

#define _POINT_LIGHTS_COUNT 64
#define EPSILON 0.0001

struct PointLight {
	vec3 color;
	vec3 position;
	vec3 attenuation;
};

struct Ray {
	vec3 origin;
	vec3 direction;
};

out vec4 FragColor;

in vec2 TexCoords;
in vec3 viewDirection;
in Ray ray;

uniform PointLight pointLights[_POINT_LIGHTS_COUNT];

uniform int nrPointLight;
uniform vec3 cameraPosition;
uniform float time;
uniform float mouse_x;
uniform float mouse_y;

float sphereSDF(vec3 pos){
	return length(pos)-1;
}

float sceneSDF(vec3 pos){
	return sphereSDF(pos);
}

vec3 calculateNormal(vec3 p){
	return normalize(vec3(
        sceneSDF(vec3(p.x + EPSILON, p.y, p.z)) - sceneSDF(vec3(p.x - EPSILON, p.y, p.z)),
        sceneSDF(vec3(p.x, p.y + EPSILON, p.z)) - sceneSDF(vec3(p.x, p.y - EPSILON, p.z)),
        sceneSDF(vec3(p.x, p.y, p.z  + EPSILON)) - sceneSDF(vec3(p.x, p.y, p.z - EPSILON))
    ));
}

void main() {
	vec3 rayDirection = normalize(ray.direction);
	FragColor = vec4(rayDirection,1.0f);
}
