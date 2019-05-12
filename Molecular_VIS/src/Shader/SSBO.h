#pragma once
#include <GL/glew.h>
#include <vector>

/**
* Wrapper for SSBO buffer (shader storage buffer object)
* T is the type of the data (struct and has to satisfy 
* allignment conditions, see ssbo doc.
**/
template<class T>
class SSBO
{
private:
	GLuint handle;
	unsigned int size;
	unsigned int target;

public:
	SSBO(unsigned int size);
	~SSBO();

	/**
	* returns capacity of buffer
	**/
	unsigned int getCapacity();

	/**
	* Uploads data to Buffer.
	* data.size()*sizeof(T) has to be less than capacity
	**/
	void uploadData(const std::vector<T> data);

	/**
	* Binds buffer to indexed buffer target (number in shader layout(..,target))
	**/
	void bindToTarget(unsigned int target);

	/**
	* Deletes the buffer
	**/
	void release();
};

