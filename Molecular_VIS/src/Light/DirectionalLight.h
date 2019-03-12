#pragma once
#include "Light.h"
class DirectionalLight :
	public Light
{
private:
	struct DirectionalLightProperties {
		glm::vec3 color;
		glm::vec3 direction;
		DirectionalLightProperties(glm::vec3 color, glm::vec3 direction) :color(color), direction(direction){}
	} properties;
protected:
	virtual void setUniform(std::shared_ptr<Shader>& shader, int index);
public:
	DirectionalLight(glm::vec3 color, glm::vec3 direction);
	~DirectionalLight();
};

