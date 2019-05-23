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


/********************************************
* IMPLEMENTATION
*********************************************/
template<class T>
SSBO<T>::SSBO(unsigned int size)
	:size(size), target(-1)
{
	glGenBuffers(1, &handle);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, handle);
	glBufferData(GL_SHADER_STORAGE_BUFFER, size*sizeof(T), NULL, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

template<class T>
SSBO<T>::~SSBO()
{
	glDeleteBuffers(1, &handle);
}
template<class T>
unsigned int SSBO<T>::getCapacity()
{
	return size;
}

template<class T>
void SSBO<T>::uploadData(const std::vector<T> data)
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, handle);
	std::cout << sizeof(data[0]);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, data.size() * sizeof(data[0]), data.data());
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

template<class T>
void SSBO<T>::bindToTarget(unsigned int target)
{
	if (this->target != target)
	{
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, target, handle);
		this->target = target;
	}
}

template<class T>
void SSBO<T>::release()
{
	glDeleteBuffers(1, &handle);
}
