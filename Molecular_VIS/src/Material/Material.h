#pragma once
#include <glm/glm.hpp>
#include "../Shader/Shader.h"

/*!
 * \brief Base class for materials.
 * 
 * Base class for materials.
 * 
 */
class Material
{
protected:
	/*!
	 * Shader that is used to draw the material.
	 * 
	 */
	std::shared_ptr<Shader> shader;
public:
	/*!
	 * Constructs material with a given shader.
	 * 
	 * \param shader Shader that is used to draw the material.
	 */
	Material(std::shared_ptr<Shader> shader);
	virtual ~Material();

	/*!
	 * Sets all material related uniform in its shader.
	 * 
	 */
	virtual void setUniforms();

	/*!
	 * Gets the shader used to draw the material.
	 * 
	 * \return Shader used to draw the material.
	 */
	virtual std::shared_ptr<Shader> getShader() final;
};

