#include "Material.h"

Material::Material(std::shared_ptr<Shader> shader):shader(shader)
{
}

Material::~Material()
{
}

void Material::setUniforms()
{
}

void Material::setUniforms(int textureUnit)
{
	setUniforms();
}


std::shared_ptr<Shader> Material::getShader()
{
	return shader;
}
