#include "Camera.h"
using namespace glm;
/*
 Creates Camera, fov in degrees
*/

Camera::Camera(float fov, float aspect, float near, float far)
{ 
	pitch = 0.0f;
	yaw = 0.0f;
	radius = 20.0f;
	position = vec3(0.0f, 0.0f, 20.0f);
	strafe = vec3(0.0f, 0.0f, 0.0f);
	vec3 up = vec3(0.0f, 1.0f, 0.0f);
	vec3 right = vec3(1.0f, 0.0f, 0.0f);
	projectionMatrix = perspective(radians(fov), aspect, near, far);
	mat4 rotation = rotate(mat4(1.0f), pitch, right)*rotate(mat4(1.0f), -yaw, up);
	viewMatrix = transpose(rotation) * translate(mat4(1.0f), -position);
	projectionViewMatrix = projectionMatrix * viewMatrix;
	inverseProjectionViewMatrix = glm::inverse(projectionViewMatrix);
}


Camera::~Camera()
{
}

glm::mat4 Camera::getInverseProjectionViewMatrix()
{
	return inverseProjectionViewMatrix;
}

mat4 Camera::getProjectionViewMatrix()
{
	return projectionViewMatrix;
}

glm::mat4 Camera::getViewMatrix()
{
	return viewMatrix;
}

glm::vec3 Camera::getPosition()
{
	return position;
}

void Camera::update(int x, int y, float zoom, bool dragging, bool strafing)
{
	mat4 rotation = mat4(1.0f);
	radius = zoom;
	if (radius < 0.2) radius = 0.2f;
	int deltaX = x - lastX;
	int deltaY = y - lastY;
	if (dragging)
	{
		float deltaAnglePitch = 2 * pi<float>()*float(deltaY) / 600.0f;
		float deltaAngleYaw = pi<float>()*float(deltaX) / 600.0f;
		pitch += deltaAnglePitch;
		yaw -= deltaAngleYaw;
		if (pitch < -pi<float>() / 2.0f + 0.01f)
		{
			pitch = -pi<float>() / 2.0f + 0.01f;
		}
		if (pitch > pi<float>() / 2.0f - 0.01f)
		{
			pitch = pi<float>() / 2.0f - 0.01f;
		}
	}
	position = radius * vec3(cosf(pitch)*sin(yaw), sinf(pitch), cosf(pitch)*cosf(yaw));

	vec3 up = vec3(0.0f, 1.0f, 0.0f);
	vec3 lookingDirection =-position;
	vec3 front = normalize(lookingDirection);
	vec3 right = normalize(cross(front, up));
	vec3 zAxis = cross(right, front);

	if (strafing)
	{
		strafe += -0.01f * float(deltaX) * right + 0.01f * float(deltaY) * zAxis;
	}

	position += strafe;

	rotation = mat4(vec4(right, 0.0f), vec4(zAxis, 0.0f), vec4(-front, 0.0f), vec4(0.0f, 0.0f, 0.0f, 1.0f));
	
	viewMatrix = transpose(rotation)*translate(mat4(1.0f),-position);

	projectionViewMatrix = projectionMatrix*viewMatrix;

	inverseProjectionViewMatrix = glm::inverse(projectionViewMatrix);

	lastX = x;
	lastY = y;
}