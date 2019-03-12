#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "Light.h"
#include "PointLight.h"
#include "DirectionalLight.h"
#include "Spotlight.h"
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
	LightManager();
	~LightManager();
	
	const static int maxDirectionalLights;
	const static int maxPointLights;
	const static int maxSpotLights;

	void createPointLight(glm::vec3 color, glm::vec3 position, glm::vec3 attenuation);
	void createDirectionalLight(glm::vec3 color, glm::vec3 direction);
	void createSpotLight(glm::vec3 color, glm::vec3 position, glm::vec3 direction, float innerOpeningAngle, float outerOpeningAngle, glm::vec3 attenuation);

	void setUniforms(const std::vector<std::shared_ptr<Shader>>& shaders);
};

