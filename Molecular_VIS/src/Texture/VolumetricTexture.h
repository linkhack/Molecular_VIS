#pragma once
#include "Texture.h"
#include "GL/glew.h"
class VolumetricTexture :
	public Texture
{
private:
	int width, height, depth;
	GLuint handle;
public:
	/**
	* Creates 3d texture with dimension (width,height,depth) and fills it with 0
	**/
	VolumetricTexture(int width, int height, int depth, GLenum internalFormat, GLenum format, GLenum type);
	virtual ~VolumetricTexture();
	virtual void bindAsTexture(int unit);
	virtual void bindAsImage(int unit);
};

