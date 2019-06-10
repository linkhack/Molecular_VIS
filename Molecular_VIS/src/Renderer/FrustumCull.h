// inspired by http://www.lighthouse3d.com/tutorials/view-frustum-culling/
#pragma once
#include "glm/glm.hpp"
#include "FrustumPlane.h"

 /*!
  * \brief Class for frustum-culling.
  * 
  */
class FrustumCull {

public:
	/*!
	 * Enum for classification of intersection
	 * 
	 */
	enum Classification { OUTSIDE, INTERSECT, INSIDE };
	
	/*!
	 * Sets the parameters needed for calculation of frustum planes. These parameters come from the
	 * camera projection
	 * 
	 * \param angle Field of view of camera in degrees.
	 * \param ratio Aspect ratio.
	 * \param nearD Near plane z.
	 * \param farD Far plane z.
	 */
	void setCamInternals(float angle, float ratio, float nearD, float farD);

	/*!
	 * Sets the parameters needed for calculation of frustum planes. These parameters come from the
	 * camera position and orientation. Calling this function updates the Culling Planes.
	 * 
	 * \param p position of camera
	 * \param l coordinates of point where the camera looks at / focus point.
	 * \param u Vector pointing up from the camera.
	 * \param l,u define the camera coordinate system.
	 */
	void setCamDef(const glm::vec3 &p, const glm::vec3 &l, const glm::vec3 &u);

	/*!
	 * Checks if a point lies inside the frustum region
	 * 
	 * \param p coordinates of the point to check
	 * \return INSIDE or OUTSIDE depending on if the point lies inside or outside of the frustum region
	 */
	Classification pointInFrustum(const glm::vec3 &p) const;

	/*!
	 * Checks the intersection of a sphere with the frustum region
	 * 
	 * \param p coordinates of the center of the sphere.
	 * \param raio radius of the sphere
	 * \return INSIDE, INTERSECT or OUTSIDE, depending if the circle lies totally inside or partially inside 
	 * or outside the frustum region
	 */
	Classification sphereInFrustum(const glm::vec3 &p, float raio) const;

private:

	FrustumPlane pl[6];

	glm::vec3 ntl, ntr, nbl, nbr, ftl, ftr, fbl, fbr;
	float nearD, farD, ratio, angle, tang;
	float nw, nh, fw, fh;

	enum {
		TOP = 0,
		BOTTOM,
		LEFT,
		RIGHT,
		NEARP,
		FARP
	};

};