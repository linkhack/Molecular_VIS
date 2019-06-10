#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "Light.h"
#include "PointLight.h"
#include "DirectionalLight.h"
#include "Spotlight.h"
/*!
 * \brief Manages the collection of lights
 * 
 * Is responsible for holding the information of the different lights and setting all the corresponding uniforms.
 */
class LightManager
{
private:
	std::vector<DirectionalLight> directionalLights;
	std::vector<PointLight> pointLights;
	std::vector<SpotLight> spotLights;
	int pointLightNumber;
	int directionalLightNumber;
	int spotLightNumber;
public:
	/*!
	 * Creates an empty LightManager
	 * 
	 */
	LightManager();
	~LightManager();
	
	/*! Maximum number of directional-lights.*/
	const static int maxDirectionalLights;
	/*! Maximum number of point-lights.*/
	const static int maxPointLights;
	/*! Maximum number of spot-lights.*/
	const static int maxSpotLights;

	/*!
	 * Creates a point-light and adds it to the manager. See PointLight.
	 * 
	 * \param color Color of the light.
	 * \param position Position of the light.
	 * \param attenuation Attenuation coefficients of the light.
	 */
	void createPointLight(glm::vec3 color, glm::vec3 position, glm::vec3 attenuation);

	/*!
	 * Creates a directional-light and adds it to the manager. See DirectionalLight.
	 * 
	 * \param color Color of the light.
	 * \param direction Direction of the light.
	 */
	void createDirectionalLight(glm::vec3 color, glm::vec3 direction);

	/*!
	 * Creates a spot-light and adds it to the manager. See SpotLight.
	 * 
	 * \param color Color of the light.
	 * \param position Position of the light.
	 * \param direction Direction of the light.
	 * \param innerOpeningAngle Inner angle in degrees.
	 * \param outerOpeningAngle Outer angle in degrees.
	 * \param attenuation Attenuation factors.
	 */
	void createSpotLight(glm::vec3 color, glm::vec3 position, glm::vec3 direction, float innerOpeningAngle, float outerOpeningAngle, glm::vec3 attenuation);

	/*!
	 * Sets the light releted uniforms for all shaders in the vector.
	 * 
	 * \param shaders Vector of shaders for which the uniforms will be set.
	 */
	void setUniforms(const std::vector<std::shared_ptr<Shader>>& shaders);
};

