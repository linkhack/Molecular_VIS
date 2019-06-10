#pragma once
#include "Light.h"

/*!
 * \brief Class for spot-lights.
 *
 */
class SpotLight :
	public Light
{
private:
	struct SpotLightProperties {
		glm::vec3 color;
		glm::vec3 position;
		glm::vec3 direction;
		float innerOpeningAngle;
		float outerOpeningAngle;
		glm::vec3 attenuation; //x = quad y=linear z=constant
		SpotLightProperties(glm::vec3 color, glm::vec3 position, glm::vec3 direction, float innerOpeningAngle, float outerOpeningAngle, glm::vec3 attenuation):
			color(color), position(position), direction(direction), innerOpeningAngle(innerOpeningAngle), outerOpeningAngle(outerOpeningAngle), attenuation(attenuation) {}
	} properties;
protected:
	/*!
	 * Sets the spot-light related uniforms in a shader. Sets following uniforms:
	 *		spotLights[index].color
	 *		spotLights[index].position
	 *		spotLights[index].attenuation
	 *		spotLights[index].direction
	 *		spotLights[index].innerOpeningAngle
	 *		spotLights[index].outerOpeningAngle
	 * \param shader Shader to be updated
	 * \param index index of the light among lights of the same type (used in shader to index light)
	 */
	virtual void setUniform(std::shared_ptr<Shader>& shader, int index);
public:
	/*!
	 * Creates a spot-light.
	 * 
	 * \param color Color of the light.
	 * \param position Position of the light.
	 * \param direction Direction of the light.
	 * \param innerOpeningAngle Inner opening angle of the light in degrees.
	 * \param outerOpeningAngle Outer opening angle of the light in degrees.
	 * \param attenuation Attenuation coefficients of the light.
	 */
	SpotLight(glm::vec3 color, glm::vec3 position, glm::vec3 direction, float innerOpeningAngle, float outerOpeningAngle, glm::vec3 attenuation);
	~SpotLight();
};

