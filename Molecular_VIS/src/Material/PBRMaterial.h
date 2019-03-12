#pragma once
#include "Material.h"
#include <random>

class PBRMaterial :
	public Material
{
private:
	glm::vec3 baseColor;
	float ambient;
	float metallic;
	float specular;
	float specularTint;
	float roughness;
	float sheen;
	float sheenTint;
	float clearcoat;
	float cleatcoatGloss;
public:
	PBRMaterial(std::shared_ptr<Shader> shader, glm::vec3 baseColor);
	PBRMaterial(std::shared_ptr<Shader> shader, glm::vec3 baseColor,float metallic, float roughness);
	PBRMaterial(std::shared_ptr<Shader> shader,glm::vec3 baseColor, float ambient, float metallic, float specular, float specularTint, float roughness, float anisotropic, float sheen, float sheenTint, float clearcoat, float clearcoatGloss);
	virtual ~PBRMaterial();

	virtual void setUniforms();
};

