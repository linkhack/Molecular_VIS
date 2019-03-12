#pragma once
#include "Light.h"

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
	virtual void setUniform(std::shared_ptr<Shader>& shader, int index);
public:
	PointLight(glm::vec3 color, glm::vec3 position, glm::vec3 attenuation);
	~PointLight();
};

