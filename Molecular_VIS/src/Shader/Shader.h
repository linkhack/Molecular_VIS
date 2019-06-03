#pragma once
#include <Gl/glew.h>
#include <glm/glm.hpp>
#include <string>
#include <fstream>
#include <iostream>
#include <unordered_map>

/*!
 * \brief Class handling shader construction and managment.
 * 
 * This class can be used to generate vertex-fragment programs or compute shader programs.
 * It handles the reading of the source files and compilation and linking of the source.
 * It also handles the setting of uniforms and can also handle the import of shaders into
 * other shaders.
 */
class Shader
{
private:
	GLuint handle;
	std::string vertexShader, fragmentShader, computeShader, geometryShader;
	std::unordered_map<std::string, GLint> locations;

	void handleError(std::string name, GLuint shaderId);
	GLuint loadStandardProgram();
	GLuint loadComputeProgram();
	bool loadShader(std::string source, GLenum shaderType, GLuint& shaderHandle);

	GLint getUniformLocation(std::string location);

	std::string readFile(std::string filePath);

	void processDirectives(std::string& source);
	void processImport(std::string& source);

public:
	/*!
	 * 
	 * Preprocesser directive for import statement.
	 * Usage in shader code:
	 * <IMPORT_DIRECTIVE> <file>
	 * 
	 * This directive gets replaced with the code in file before compilation.
	 * 
	 */
	static const std::string IMPORT_DIRECTIVE;

	/*!
	 * Path to the shader folder
	 * 
	 */
	static const std::string SHADER_DIRECTORY;
	/*!
	 * Creates a vertex - fragment shader program
	 * 
	 * \param vertexShader filename of vertex shader, has to be located in SHADER::SHADER_DIRECTORY
	 * \param fragmentShader filename of fragment shader, has to be located in SHADER::SHADER_DIRECTORY
	 */
	Shader(std::string vertexShader, std::string fragmentShader);

	/*!
	 * Creates a compute shader program 
	 * 
	 * \param computeShader filename of compute shader, has to be located in SHADER::SHADER_DIRECTORY
	 */
	Shader(std::string computeShader);

	/*!
	 * Set a vec3 uniform
	 * 
	 * \param uniform string of uniform name
	 * \param value glm::vec3 value to be set
	 */
	void setUniform(std::string uniform, const glm::vec3& value);
	/*!
	 * Set a vec3 uniform
	 * 
	 * \param location GLint location of uniform
	 * \param value glm::vec3 value to be set
	 */
	void setUnifrom(GLint location, const glm::vec3& value);
	/*!
	 * Set a uvec3 uniform
	 *
	 * \param uniform string of uniform name
	 * \param value glm::uvec3 value to be set
	 */
	void setUniform(std::string uniform, const glm::uvec3 & value);
	/*!
	 * Set a uvec3 uniform
	 *
	 * \param location GLint location of uniform
	 * \param value glm::uvec3 value to be set
	 */
	void setUnifrom(GLint location, const glm::uvec3 & value);
	/*!
	 * Set a int uniform
	 *
	 * \param uniform string of uniform name
	 * \param value int value to be set
	 */
	void setUniform(std::string uniform, const int value);
	/*!
	 * Set a int uniform
	 *
	 * \param location GLint location of uniform
	 * \param value int value to be set
	 */
	void setUniform(GLint location, const int value);
	/*!
	 * Set a float uniform
	 *
	 * \param uniform string of uniform name
	 * \param value float value to be set
	 */
	void setUniform(std::string uniform, const float value);
	/*!
	 * Set a float uniform
	 *
	 * \param location GLint location of uniform
	 * \param value float value to be set
	 */
	void setUniform(GLint location, const float value);
	/*!
	 * Set a mat4 uniform
	 *
	 * \param uniform string of uniform name
	 * \param mat glm::mat4 value to be set
	 */
	void setUniform(std::string uniform, const glm::mat4& mat);
	/*!
	 * Set a mat4 uniform
	 *
	 * \param location GLint location of uniform
	 * \param mat glm::mat4 value to be set
	 */
	void setUniform(GLint location, const glm::mat4& mat);
	/*!
	 * Set a uvec3 uniform
	 *
	 * \param uniform string of uniform name
	 * \param mat glm::uvec3 value to be set
	 */
	void setUniform(std::string uniform, const glm::mat3& mat);

	/*!
	 * Set a mat3 uniform
	 *
	 * \param location GLint location of uniform
	 * \param mat glm::mat3 value to be set
	 */
	void setUniform(GLint location, const glm::mat3& mat);

	/*!
	 * Sets the shader program as active.
	 * 
	 */
	void use();

	/*!
	 * Disables current active shader program.
	 * 
	 */
	void unuse();
	~Shader();
};

