#version 440 core
layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTexCoords;

struct Ray {
	vec3 origin;
	vec3 direction;
};

out vec2 TexCoords;
out Ray ray;
out float distance;

uniform vec3 cameraPosition;
uniform mat4 viewMatrix;
uniform mat4 inversePVMatrix;

void main()
{
    TexCoords = aTexCoords;
	gl_Position = vec4(aPos, 1.0f);    

	//Ray generation
	vec4 farPlane = inversePVMatrix*vec4(aPos.xy,1.0f,1.0f);
	farPlane = farPlane/farPlane.w;

	vec4 nearPlane = inversePVMatrix*vec4(aPos.xy,-1.0f,1.0f);
	nearPlane = nearPlane/nearPlane.w;
	
	//direction and origing
	/**
	// Complicated way, but origin = cam
	// ray.direction = (farPlane.xyz-nearPlane.xyz);
	// ray.origin = nearPlane.xyz;
	**/
	distance = sqrt(dot(vec3(farPlane),vec3(nearPlane)));
	ray.direction = farPlane.xyz-cameraPosition;
	ray.origin = cameraPosition;
}