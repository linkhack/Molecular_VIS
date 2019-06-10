#pragma once
#include <GL/glew.h>
#include <vector>

/*!
 * \brief Wrapper for SSBO buffer (shader storage buffer object)
 * T is the type of the data (struct and has to satisfy
 * allignment conditions, see ssbo doc.)
 * 
 */
template<class T>
class SSBO
{
private:
	GLuint handle;
	unsigned int size;
	unsigned int target;
	bool isInit = false;
public:
	/*!
	 * Empty constructor, does nothing.
	 * 
	 */
	SSBO();
	/*!
	 * Creates SSBO with given capacity.
	 * 
	 * \param size The number of elements of type T that can be saved in this SSBO.
	 */
	SSBO(unsigned int size);
	~SSBO();

	/**
	* returns capacity of buffer
	**/
	unsigned int getCapacity();

	/**
	* Uploads data to Buffer.
	* data.size()) has to be less than capacity
	**/
	void uploadData(const std::vector<T>& data);

	/**
	* Binds buffer to indexed buffer target (number in shader layout(..,target))
	**/
	void bindToTarget(unsigned int target);

	/**
	* Deletes the buffer
	**/
	void release();

	/*!
	* clears the Buffer
	*/
	void clear();
};


/********************************************
* IMPLEMENTATION
*********************************************/

template<class T>
SSBO<T>::SSBO()
	:size(0), target(-1)
{}

template<class T>
SSBO<T>::SSBO(unsigned int size)
	:size(size), target(-1), isInit(true)
{
	glGenBuffers(1, &handle);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, handle);
	std::cout << handle << '\n';
	glBufferData(GL_SHADER_STORAGE_BUFFER, size*sizeof(T), NULL, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

template<class T>
SSBO<T>::~SSBO()
{
	if (isInit)
	{
		std::cout << "Deleted SSBO with handle: " << handle << '\n';
		glDeleteBuffers(1, &handle);
	}
}
template<class T>
unsigned int SSBO<T>::getCapacity()
{
	return size;
}

template<class T>
void SSBO<T>::uploadData(const std::vector<T>& data)
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

template<class T>
void SSBO<T>::clear()
{
	glBufferData(GL_SHADER_STORAGE_BUFFER, size * sizeof(T), NULL, GL_DYNAMIC_DRAW);
}
