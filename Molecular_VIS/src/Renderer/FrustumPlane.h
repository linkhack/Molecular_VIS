// inspired by http://www.lighthouse3d.com/tutorials/view-frustum-culling/
#pragma once
#include "glm/glm.hpp"

/*!
 * \brief Class for Frustum plane
 * 
 */
class FrustumPlane {
private:
	glm::vec3 normal, point;
	float d;

public:
	/*!
	 * Constructs frustum plane from three points. The orientation of the plane is defined by the order
	 * of the points, assumed counter-clockwise
	 * 
	 * \param v1 first point
	 * \param v2 second point
	 * \param v3 third point
	 */
	FrustumPlane(const glm::vec3 &v1, const glm::vec3 &v2, const glm::vec3 &v3);
	FrustumPlane(void);	
	/*!
	 * Updates frustum plane from three points. The orientation of the plane is defined by the order
	 * of the points, assumed counter-clockwise
	 *
	 * \param v1 First point.
	 * \param v2 Second point.
	 * \param v3 Third point.
	 */
	void set3Points(const glm::vec3 &v1, const glm::vec3 &v2, const glm::vec3 &v3);
	/*!
	 * Calculates signed distance from plane.
	 * 
	 * \param p Coordinates of point to which the distance will be calculated.
	 * \return The signed distance from the plane to the point.
	 */
	float distance(const glm::vec3 &p) const;
};


