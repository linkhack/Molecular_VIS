#pragma once
#include "Light.h"

/*!
 * \brief Class for point-lights.
 * 
 */
class PointLight :
	public Light
{
private:
	struct PointLightProperties {
		glm::vec3 color;
		glm::vec3 position;
		glm::vec3 attenuation; //x = quad y=linear z=constant
		PointLightProperties(glm::vec3 color, glm::vec3 position, glm::vec3 attenuation) :color(color), position(position), attenuation(attenuation) {}
	} properties;

protected:
	/*!
	 * Sets the point-light related uniforms in a shader. Sets following uniforms:
	 *		pointLights[index].color
	 *		pointLights[index].position
	 *		pointLights[index].attenuation
	 * \param shader Shader to be updated
	 * \param index index of the light among lights of the same type (used in shader to index light)
	 */
	virtual void setUniform(std::shared_ptr<Shader>& shader, int index);
public:
	/*!
	 * Creates a point-light.
	 * 
	 * \param color Color of the light.
	 * \param position Position of the light.
	 * \param attenuation Attenuation coefficients of the light.ss
	 */
	PointLight(glm::vec3 color, glm::vec3 position, glm::vec3 attenuation);
	~PointLight();
};

