#pragma once
#include <string>

/*!
 * \brief Abstract base class for textures.
 * 
 */
class Texture {
public:
	virtual ~Texture() {};
private:
	virtual void bindAsTexture(int unit) = 0;
	virtual void bindAsImage(int unit) = 0;
};