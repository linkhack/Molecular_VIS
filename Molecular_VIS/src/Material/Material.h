#pragma once
#include <glm/glm.hpp>
#include "../Shader/Shader.h"
class Material
{
protected:
	std::shared_ptr<Shader> shader;
public:
	Material(std::shared_ptr<Shader> shader);
	virtual ~Material();
	virtual void setUniforms();
	virtual void setUniforms(int textureUnit);
	virtual std::shared_ptr<Shader> getShader() final;
};

