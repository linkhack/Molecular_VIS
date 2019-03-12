#pragma once
#include <glm/glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <iostream>
class Camera
{
private:
	float pitch, yaw, radius;
	int lastX, lastY;
	glm::vec3 position;
	glm::vec3 strafe;
	glm::mat4 projectionMatrix;
	glm::mat4 viewMatrix;
	glm::mat4 projectionViewMatrix;
	glm::mat4 inverseProjectionViewMatrix;
public:
	Camera(float fov, float aspect, float near, float far);
	~Camera();

	glm::mat4 getInverseProjectionViewMatrix();
	glm::mat4 getProjectionViewMatrix();
	glm::mat4 getViewMatrix();
	glm::vec3 getPosition();
	void update(int x, int y, float zoom, bool dragging, bool strafing);
};

