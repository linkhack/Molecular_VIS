#version 430 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 3) in vec3 offset;

uniform mat4 viewProjectionMatrix;
uniform mat4 modelMatrix;
uniform mat3 normalMatrix;

out struct VertexData {
	vec3 worldPosition;
	vec3 normal;
} vert;

void main() {
	
	vert.normal = normalize(normalMatrix*normal);

	vec4 position_world =modelMatrix * vec4(position,1.0f);
	vert.worldPosition = position_world.xyz;
	gl_Position = viewProjectionMatrix * position_world;
}