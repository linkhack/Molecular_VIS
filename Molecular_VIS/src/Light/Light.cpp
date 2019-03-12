#include "Light.h"



void Light::setUniform(std::shared_ptr<Shader>& shader, int index)
{
}

Light::Light(){
}


Light::~Light()
{
}

void Light::setUniforms(const std::vector<std::shared_ptr<Shader>>& shaders, int index)
{
	for (std::shared_ptr<Shader> shader : shaders) 
	{
		setUniform(shader, index);
	}
}
