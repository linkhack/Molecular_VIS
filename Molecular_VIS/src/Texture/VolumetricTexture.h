#pragma once
#include "Texture.h"
#include "GL/glew.h"

/*!
 * \brief Wrapper class for GL_TEXTURE_3D
 * 
 */
class VolumetricTexture :
	public Texture
{
private:
	int width, height, depth;
	GLuint handle;
	GLenum internalFormat;
public:
	/*!
	 *  Creates 3d texture. 
	 * 
	 * \param width, height, depth Dimensions of texture.
	 * \param internalFormat Internal format of the texture. See openGL documentation.
	 * \param format Format of data.
	 * \param type Type of data.
	 */
	VolumetricTexture(int width, int height, int depth, GLenum internalFormat, GLenum format, GLenum type);
	VolumetricTexture() = default;
	virtual ~VolumetricTexture();
	/*!
	 * Binds the texture as texture to texture unit.
	 * 
	 * \param unit Texture unit to bind to.
	 */
	virtual void bindAsTexture(int unit);

	/*!
	 * Binds the texture as image for image load/store operations (in compute shaders etc.)
	 * 
	 * \param unit Location to bind the image to.
	 */
	virtual void bindAsImage(int unit);
};

