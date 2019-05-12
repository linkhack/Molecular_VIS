#include "VolumetricTexture.h"


VolumetricTexture::VolumetricTexture(int width, int height, int depth)
	:width(width), height(height), depth(depth)
{
	glCreateTextures(GL_TEXTURE_3D, 1, &handle);
	glBindTexture(GL_TEXTURE_3D, handle);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_R32F, width, height, depth, 0, GL_R32F, GL_FLOAT, NULL);
	// TODO: texture params
	glBindTexture(GL_TEXTURE_3D, 0);
}
VolumetricTexture::~VolumetricTexture()
{
}

void VolumetricTexture::bindAsTexture(int unit)
{
	glBindTexture(GL_TEXTURE_3D, handle);
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_3D, 0);
}

void VolumetricTexture::bindAsImage(int unit)
{

}


