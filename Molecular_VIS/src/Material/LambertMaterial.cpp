#include "LambertMaterial.h"



LambertMaterial::~LambertMaterial()
{
}

LambertMaterial::LambertMaterial(std::shared_ptr<Shader> shader, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float specularCoefficient)
	:Material(shader),ambient(ambient),diffuse(diffuse),specular(specular),specularCoefficient(specularCoefficient), color(glm::vec3(1.0f))
{
}

LambertMaterial::LambertMaterial(std::shared_ptr<Shader> shader) : LambertMaterial(shader, glm::vec3(0.05f), glm::vec3(0.9f), glm::vec3(0.3f), 10.0f)
{
}

void LambertMaterial::setColor(glm::vec3 color)
{
	this->color = color;
}

void LambertMaterial::setUniforms()
{
	shader->use();
	shader->setUniform("materialColor", color);
	shader->setUniform("materialCoefficients.ambient", ambient);
	shader->setUniform("materialCoefficients.diffuse", diffuse);
	shader->setUniform("materialCoefficients.specular", specular);
	shader->setUniform("materialCoefficients.specularCoefficient", specularCoefficient);
	shader->unuse();
}
