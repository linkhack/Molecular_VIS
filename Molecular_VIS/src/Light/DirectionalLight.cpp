#include "DirectionalLight.h"



void DirectionalLight::setUniform(std::shared_ptr<Shader>& shader, int index)
{
	shader->use();
	shader->setUniform("directionalLights["+std::to_string(index)+"].color", properties.color);
	shader->setUniform("directionalLights[" + std::to_string(index) + "].direction", properties.direction);
	shader->unuse();
}

DirectionalLight::DirectionalLight(glm::vec3 color, glm::vec3 direction):properties(color,glm::normalize(direction))
{
}


DirectionalLight::~DirectionalLight()
{
}

