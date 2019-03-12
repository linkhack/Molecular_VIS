#pragma once
#include "Light.h"
class SpotLight :
	public Light
{
private:
	struct SpotLightProperties {
		glm::vec3 color;
		glm::vec3 position;
		glm::vec3 direction;
		float innerOpeningAngle;
		float outerOpeningAngle;
		glm::vec3 attenuation; //x = quad y=linear z=constant
		SpotLightProperties(glm::vec3 color, glm::vec3 position, glm::vec3 direction, float innerOpeningAngle, float outerOpeningAngle, glm::vec3 attenuation):
			color(color), position(position), direction(direction), innerOpeningAngle(innerOpeningAngle), outerOpeningAngle(outerOpeningAngle), attenuation(attenuation) {}
	} properties;
protected:
	virtual void setUniform(std::shared_ptr<Shader>& shader, int index);
public:
	SpotLight(glm::vec3 color, glm::vec3 position, glm::vec3 direction, float innerOpeningAngle, float outerOpeningAngle, glm::vec3 attenuation);
	~SpotLight();
};

