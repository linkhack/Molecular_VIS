#pragma once
#include "../Shader/Shader.h"

class Light
{
protected:
	virtual void setUniform(std::shared_ptr<Shader>& shader, int index);
public:
	Light();
	virtual ~Light();

	virtual void setUniforms(const std::vector<std::shared_ptr<Shader>>& shaders, int index) final;
};

