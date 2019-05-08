#pragma once
#include "../Material/Material.h"
class LambertMaterial :
	public Material
{
private:
	glm::vec3 color;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float specularCoefficient;
public:
	LambertMaterial(std::shared_ptr<Shader> shader, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float specularCoefficient);
	LambertMaterial(std::shared_ptr<Shader> shader);
	~LambertMaterial();
	virtual void setColor(glm::vec3 color);
	virtual void setUniforms();
};

