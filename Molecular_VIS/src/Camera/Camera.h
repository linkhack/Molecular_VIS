#pragma once
#include <glm/glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <iostream>
/*!
 * \brief A class for an arc-ball camera.
 *
 * This class defines a camera and calculates the needed transformations, projections and vectors derived 
 * from the camera. The camera is an arc ball camera.
 * 
 */
class Camera
{
private:
	float pitch, yaw, radius;
	int lastX, lastY;
	glm::vec3 position;
	glm::vec3 strafe;
	glm::vec3 normalizedDirection;
	glm::mat4 projectionMatrix;
	glm::mat4 viewMatrix;
	glm::mat4 projectionViewMatrix;
	glm::mat4 inverseProjectionViewMatrix;
public:
	/*!
	 * Creates a camera with given parameter, which define the projection matrix.
	 * 
	 * \param fov Field of view of projection in degrees.
	 * \param aspect Aspect ratio of projection.
	 * \param near Near plane z.
	 * \param far Far plane z.
	 */
	Camera(float fov, float aspect, float near, float far);
	~Camera();

	/*!
	 * Gets the inverse of the projection view matrix. Transforms from screen space to world space.
	 * 
	 * \return glm::mat4 inverse of the projection view matrix
	 */
	glm::mat4 getInverseProjectionViewMatrix();
	/*!
	 * Gets the the projection view matrix. Transforms from world space to screen space.
	 *
	 * \return glm::mat4 the projection view matrix
	 */
	glm::mat4 getProjectionViewMatrix();

	/*!
	 * Gets the the view matrix. Transforms from world space to camera space.
	 *
	 * \return glm::mat4 the view matrix
	 */
	glm::mat4 getViewMatrix();

	/*!
	 * Gets the position of the camera.
	 * 
	 * \return glm::vec3 Pposition of camera.
	 */
	glm::vec3 getPosition();

	/*!
	 * Return the normalized view axis direction.
	 * 
	 * \return glm::vec3 Normalized view direction. Has length 1.
	 */
	glm::vec3 getDirection();

	/*!
	 * Updates the camera position and direction. Has to be updated often as relative movement is used, therefor if
	 * the camera wasn't updated for some time the position and direction can jump by a lot.
	 * 
	 * \param x x coordinate of mouse in pixel
	 * \param y y coordinates of mouse in pixel
	 * \param zoom zoom radius, corresponds to the distance from the focus point to the camera.
	 * \param dragging if the movement should change the orientation (dragging the camera around on a ball)
	 * \param strafing if the camera position should be moved in the tangential plane on the sphere where the camera is placed.
	 * (strafing movement)
	 */
	void update(int x, int y, float zoom, bool dragging, bool strafing);
};

