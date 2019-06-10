#pragma once
#include "Light.h"

/*!
 * \brief Class defining directional lights
 * 
 */
class DirectionalLight :
	public Light
{
private:
	/*!
	 * Struct of the properties of a directional light.
	 * 
	 */
	struct DirectionalLightProperties {
		glm::vec3 color;
		glm::vec3 direction;
		DirectionalLightProperties(glm::vec3 color, glm::vec3 direction) :color(color), direction(direction){}
	} properties;
protected:

	/*!
	 * Sets the directional light related uniforms in a shader. Sets following uniforms:
	 * 	"directionalLights[index].color"
	 *  "directionalLights[index].direction"
	 * \param shader Shader to be updated
	 * \param index index of the light among lights of the same type (used in shader to index light)
	 */
	virtual void setUniform(std::shared_ptr<Shader>& shader, int index);
public:
	/*!
	 * Creates directional light.
	 * 
	 * \param color Color of the light. The r,g,b components should be between 0 and 1.
	 * \param direction The direction of the light. This gets normalized.
	 */
	DirectionalLight(glm::vec3 color, glm::vec3 direction);
	~DirectionalLight();
};

