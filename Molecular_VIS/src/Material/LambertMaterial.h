#pragma once
#include "../Material/Material.h"

/*!
 * \brief Class for a material with lambert brdf.
 * 
 */
class LambertMaterial :
	public Material
{
private:
	glm::vec3 color;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float specularCoefficient;
public:
	/*!
	 * Construct material with lambert brdf.
	 * 
	 * \param shader Shader that is used to draw this material.
	 * \param ambient Ambient color.
	 * \param diffuse Diffuse color.
	 * \param specular Specular color.
	 * \param specularCoefficient specular coeffiecent, sometimes also called shineness.
	 */
	LambertMaterial(std::shared_ptr<Shader> shader, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float specularCoefficient);
	/*!
	 * Construut material with lambert brdf with default parameters.
	 * 
	 * \param shader Shader that is used to draw this material.
	 */
	LambertMaterial(std::shared_ptr<Shader> shader);
	~LambertMaterial();
	/*!
	 * Sets the base color of this material
	 * 
	 * \param color Base color.
	 */
	virtual void setColor(glm::vec3 color);

	/*!
	 * Sets all material related uniforms in the shader of this material. 
	 * 
	 */
	virtual void setUniforms();
};

