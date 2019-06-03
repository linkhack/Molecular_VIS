#pragma once
#include "../Shader/Shader.h"

/*!
 * \brief Base class for lights
 * 
 */
class Light
{
protected:
	/*!
	 * Uniforms to update for a particular light and shader. Has to be overwritten.
	 * 
	 * \param shader shader to be updated.
	 * \param index index index of the light among lights of the same type (used in shader to index light)
	 */
	virtual void setUniform(std::shared_ptr<Shader>& shader, int index);
public:
	Light();
	virtual ~Light();
	/*!
	 * Updates all light related uniform for all shaders in shaders-vector for this light.
	 * Internally calls void setUniform(std::shared_ptr<Shader>& shadeer, int index);
	 * 
	 * \param shaders Vectors of all shaders to be uodated.
	 * \param index index of the light among lights of the same type (used in shader to index light)
	 */
	virtual void setUniforms(const std::vector<std::shared_ptr<Shader>>& shaders, int index) final;
};

