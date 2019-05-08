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
	VolumetricTexture(int width, int height, int depth);
	virtual ~VolumetricTexture();
	virtual void bind(int unit);
};

