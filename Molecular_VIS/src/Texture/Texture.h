#pragma once
#include <string>

class Texture {
public:
	virtual ~Texture() {};
private:
	virtual void bind(int unit) = 0;
};