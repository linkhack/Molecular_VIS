#pragma once
#include "Material.h"
#include <random>

/*!
 * \brief Class for a physically based brdf. Inspired by Disney's BRDF.
 *
 *  See https://disney-animation.s3.amazonaws.com/library/s2012_pbs_disney_brdf_notes_v2.pdf
 */
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
	/*!
	 * Constructs BRDF. For meaning of parameters see: https://disney-animation.s3.amazonaws.com/library/s2012_pbs_disney_brdf_notes_v2.pdf
	 * 
	 * \param shader Shader used to draw this material.
	 * \param baseColor
	 * \param ambient
	 * \param metallic
	 * \param specular
	 * \param specularTint
	 * \param roughness
	 * \param anisotropic
	 * \param sheen
	 * \param sheenTint
	 * \param clearcoat
	 * \param clearcoatGloss
	 */
	PBRMaterial(std::shared_ptr<Shader> shader, glm::vec3 baseColor, float ambient, float metallic, float specular, float specularTint, float roughness, float anisotropic, float sheen, float sheenTint, float clearcoat, float clearcoatGloss);
	/*!
	 * \overload
	 */
	PBRMaterial(std::shared_ptr<Shader> shader, glm::vec3 baseColor);
	/*!
	 * \overload
	 */
	PBRMaterial(std::shared_ptr<Shader> shader, glm::vec3 baseColor,float metallic, float roughness);
	virtual ~PBRMaterial();

	/*!
	 * Sets all material related uniforms in the shader of this material.
	 *
	 */
	virtual void setUniforms();
};

