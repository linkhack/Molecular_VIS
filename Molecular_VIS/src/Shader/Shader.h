#pragma once
#include <Gl/glew.h>
#include <glm/glm.hpp>
#include <string>
#include <fstream>
#include <iostream>
#include <unordered_map>

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

	static const std::string IMPORT_DIRECTIVE;
	static const std::string SHADER_DIRECTORY;
public:
	Shader();
	Shader(std::string vertexShader, std::string fragmentShader);
	Shader(std::string computeShader);


	void setUniform(std::string uniform, const glm::vec3& value);
	void setUnifrom(GLint location, const glm::vec3& value);
	void setUniform(std::string uniform, const glm::uvec3 & value);
	void setUnifrom(GLint location, const glm::uvec3 & value);
	void setUniform(std::string uniform, const int value);
	void setUniform(GLint location, const int value);
	void setUniform(std::string uniform, const float value);
	void setUniform(GLint location, const float value);
	void setUniform(std::string uniform, const glm::mat4& mat);
	void setUniform(GLint location, const glm::mat4& mat);
	void setUniform(std::string uniform, const glm::mat3& mat);
	void setUniform(GLint location, const glm::mat3& mat);
	void use();
	void unuse();
	~Shader();
};

