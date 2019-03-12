#include "PointLight.h"



void PointLight::setUniform(std::shared_ptr<Shader>& shader, int index)
{
	shader->use();
	shader->setUniform("pointLights[" + std::to_string(index) + "].color", properties.color);
	shader->setUniform("pointLights[" + std::to_string(index) + "].position", properties.position);
	shader->setUniform("pointLights[" + std::to_string(index) + "].attenuation", properties.attenuation);
	shader->unuse();
}

PointLight::PointLight(glm::vec3 color, glm::vec3 position, glm::vec3 attenuation):properties(color,position,attenuation)
{
}


PointLight::~PointLight()
{
}
