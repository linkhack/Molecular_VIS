#include "SSBO.h"

template<class T>
SSBO<T>::SSBO(unsigned int size)
	:size(size), target(0)
{
	glGenBuffers(1, &handle);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, handle);
	glBufferData(GL_SHADER_STORAGE_BUFFER, size, 0, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

template<class T>
SSBO<T>::~SSBO()
{
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
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, data.size() * sizeof(T), data.data());
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
