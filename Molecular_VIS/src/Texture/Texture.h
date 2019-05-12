#pragma once
#include <string>

class Texture {
public:
	virtual ~Texture() {};
private:
	virtual void bindAsTexture(int unit) = 0;
	virtual void bindAsImage(int unit) = 0;
};