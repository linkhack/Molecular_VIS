#include "PBRMaterial.h"

/*
; Random Material
*/
PBRMaterial::PBRMaterial(std::shared_ptr<Shader> shader,glm::vec3 baseColor):Material(shader)
{
	this->baseColor = baseColor;
	std::random_device rd;
	std::mt19937 rng(rd());
	std::uniform_real_distribution<float> dist(0.0f, 1.0f);
	this->ambient = 0.1f;
	this->metallic = 0.1f;
	this->specular = dist(rng);;
	this->specularTint = dist(rng);
	this->roughness = 0.5f;
	this->sheen = dist(rng);;
	this->sheenTint = dist(rng);
	this->clearcoat = dist(rng);
	this->cleatcoatGloss = dist(rng);
}

PBRMaterial::PBRMaterial(std::shared_ptr<Shader> shader, glm::vec3 baseColor, float metallic, float roughness) :Material(shader)
{
	this->baseColor = baseColor;
	std::random_device rd;
	std::mt19937 rng(rd());
	std::uniform_real_distribution<float> dist(0.0f, 1.0f);
	this->ambient = 0.1f;
	this->metallic = metallic;
	this->specular = dist(rng);
	this->specularTint = dist(rng);
	this->roughness = roughness;
	this->sheen = dist(rng);
	this->sheenTint = dist(rng);
	this->clearcoat = dist(rng);
	this->cleatcoatGloss = dist(rng);
}

/*
	All the coefficients
*/
PBRMaterial::PBRMaterial(std::shared_ptr<Shader> shader, glm::vec3 baseColor, float ambient, float metallic, float specular, float specularTint, float roughness, float anisotropic, float sheen, float sheenTint, float clearcoat, float clearcoatGloss) :Material(shader)
{
	this->baseColor = baseColor;
	this->ambient = ambient;
	this->metallic = metallic;
	this->specular = specular;
	this->specularTint = specularTint;
	this->roughness = roughness;
	this->sheen = sheen;
	this->sheenTint = sheenTint;
	this->clearcoat = clearcoat;
	this->cleatcoatGloss = clearcoatGloss;
}

PBRMaterial::~PBRMaterial()
{
}

void PBRMaterial::setUniforms()
{
	shader->use();
	shader->setUniform("materialCoefficients.baseColor", baseColor);
	shader->setUniform("materialCoefficients.ambient", ambient);
	shader->setUniform("materialCoefficients.metallic", metallic);
	shader->setUniform("materialCoefficients.specularTint", specularTint);
	shader->setUniform("materialCoefficients.roughness", roughness);
	shader->setUniform("materialCoefficients.sheen", sheen);
	shader->setUniform("materialCoefficients.sheenTint", sheenTint);
	shader->setUniform("materialCoefficients.clearcoat", clearcoat);
	shader->setUniform("materialCoefficients.cleatcoatGloss", cleatcoatGloss);
	shader->unuse();
}
