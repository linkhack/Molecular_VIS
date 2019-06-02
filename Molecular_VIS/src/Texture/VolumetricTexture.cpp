#include "VolumetricTexture.h"
#include <glm/glm.hpp>


VolumetricTexture::VolumetricTexture(int width, int height, int depth, GLenum internalFormat, GLenum format, GLenum type)
	:width(width), height(height), depth(depth), internalFormat(internalFormat)
{
	glCreateTextures(GL_TEXTURE_3D, 1, &handle);
	glBindTexture(GL_TEXTURE_3D, handle);
	glTexImage3D(GL_TEXTURE_3D, 0, internalFormat, width, height, depth, 0, format, type, NULL);

	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
	float color[] = { 0.5f,0.0f,0.0f,0.0f };
	glTexParameterfv(GL_TEXTURE_3D, GL_TEXTURE_BORDER_COLOR, color);
	glBindTexture(GL_TEXTURE_3D, 0);
}
VolumetricTexture::~VolumetricTexture()
{
}

void VolumetricTexture::bindAsTexture(int unit)
{
	
	glBindTexture(GL_TEXTURE_3D, handle);
	glActiveTexture(GL_TEXTURE0 + unit);
	//glBindTexture(GL_TEXTURE_3D, 0);
}

void VolumetricTexture::bindAsImage(int unit)
{
	glBindTexture(GL_TEXTURE_3D, handle);
	glBindImageTexture(unit, handle, 0, GL_TRUE, 0, GL_READ_WRITE, internalFormat);
}


