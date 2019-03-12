#include "SpotLight.h"

SpotLight::~SpotLight()
{
}


void SpotLight::setUniform(std::shared_ptr<Shader>& shader, int index)
{
	shader->use();
	shader->setUniform("spotLights[" + std::to_string(index) + "].color", properties.color);
	shader->setUniform("spotLights[" + std::to_string(index) + "].position", properties.position);
	shader->setUniform("spotLights[" + std::to_string(index) + "].attenuation", properties.attenuation);
	shader->setUniform("spotLights[" + std::to_string(index) + "].direction", properties.direction);
	shader->setUniform("spotLights[" + std::to_string(index) + "].innerOpeningAngle", properties.innerOpeningAngle);
	shader->setUniform("spotLights[" + std::to_string(index) + "].outerOpeningAngle", properties.outerOpeningAngle);
	shader->unuse();
}

SpotLight::SpotLight(glm::vec3 color, glm::vec3 position, glm::vec3 direction, float innerOpeningAngle, float outerOpeningAngle, glm::vec3 attenuation):
	properties(color,position,glm::normalize(direction),glm::radians(innerOpeningAngle),glm::radians(outerOpeningAngle),attenuation)
{
}
