#include "Shader.h"

const std::string Shader::IMPORT_DIRECTIVE = "%%import";
const std::string Shader::SHADER_DIRECTORY = "./assets/shader/";

void Shader::handleError(std::string name,GLuint shaderId)
{
	GLint maxLength = 0;
	glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &maxLength);

	// The maxLength includes the NULL character
	std::vector<GLchar> errorLog(maxLength);
	glGetShaderInfoLog(shaderId, maxLength, &maxLength, &errorLog[0]);

	// Exit with failure.
	glDeleteShader(shaderId); // Don't leak the shader.

	//Log 
	std::cout << "Failed to load shader " << name << std::endl;
	for (GLchar c : errorLog) {
		std::cout << c;
	}
}

GLuint Shader::loadStandardProgram()
{
	GLuint vertexShader;
	GLuint fragmentShader;
	GLuint program;
	if (!loadShader(this->vertexShader,GL_VERTEX_SHADER,vertexShader)) {
		handleError(this->vertexShader, vertexShader);
		glDeleteShader(vertexShader);
		system("PAUSE");
		exit(1);
	}
	if (!loadShader(this->fragmentShader, GL_FRAGMENT_SHADER, fragmentShader)) {
		handleError(this->fragmentShader, fragmentShader);
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
		system("PAUSE");
		exit(1);
	}

	//Create Program
	program = glCreateProgram();

	// Attach our shaders to our program
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);

	// Link our program
	glLinkProgram(program);

	// Note the different functions here: glGetProgram* instead of glGetShader*.
	GLint isLinked = 0;
	glGetProgramiv(program, GL_LINK_STATUS, (int *)&isLinked);
	if (isLinked == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<GLchar> infoLog(maxLength);
		glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

		// We don't need the program anymore.
		glDeleteProgram(program);
		// Don't leak shaders either.
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		// Use the infoLog as you see fit.
		std::cout << "Failed to link shader " << this->vertexShader << " and " << this->fragmentShader<< std::endl;
		for (GLchar c : infoLog) {
			std::cout << c;
		}
		// In this simple program, we'll just leave
		system("PAUSE");
		exit(1);
	}
	// Always detach shaders after a successful link.
	glDetachShader(program, vertexShader);
	glDetachShader(program, fragmentShader);

	return program;
}

GLuint Shader::loadComputeProgram()
{
	GLuint computeShader;
	GLuint program;
	if (!loadShader(this->computeShader, GL_VERTEX_SHADER, computeShader)) {
		handleError(this->vertexShader, computeShader);
		glDeleteShader(computeShader);
		system("PAUSE");
		exit(1);
	}

	//Create Program
	program = glCreateProgram();

	// Attach our shaders to our program
	glAttachShader(program, computeShader);

	// Link our program
	glLinkProgram(program);

	// Note the different functions here: glGetProgram* instead of glGetShader*.
	GLint isLinked = 0;
	glGetProgramiv(program, GL_LINK_STATUS, (int *)&isLinked);
	if (isLinked == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<GLchar> infoLog(maxLength);
		glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

		// We don't need the program anymore.
		glDeleteProgram(program);
		// Don't leak shaders either.
		glDeleteShader(computeShader);

		// Use the infoLog as you see fit.
		std::cout << "Failed to link shader " << this->vertexShader << " and " << this->fragmentShader << std::endl;
		for (GLchar c : infoLog) {
			std::cout << c;
		}
		// In this simple program, we'll just leave
		system("PAUSE");
		exit(1);
	}
	// Always detach shaders after a successful link.
	glDetachShader(program, computeShader);

	return program;
}


bool Shader::loadShader(std::string filePath, GLenum shaderType, GLuint & shaderHandle)
{
	std::string shaderSource = readFile(SHADER_DIRECTORY + filePath);
	shaderHandle = glCreateShader(shaderType);
	const GLchar *source = (const GLchar *)shaderSource.c_str();
	glShaderSource(shaderHandle, 1, &source, 0);
	glCompileShader(shaderHandle);

	GLint isCompiled = 0;
	glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &isCompiled);

	return isCompiled == GL_TRUE;
}

GLint Shader::getUniformLocation(std::string location)
{
	auto search = locations.find(location);
	//GLint locationId = glGetUniformLocation(handle, location.c_str());
	//locations.insert(std::make_pair(location, locationId));
	//return locationId;
	if (search != locations.end()) 
	{
		return search->second;
	}
	else
	{
		GLint locationId = glGetUniformLocation(handle, location.c_str());
		locations.insert(std::make_pair(location, locationId));
		return locationId;
	}
}

std::string Shader::readFile(std::string filePath)
{
	std::ifstream shaderFile;
	
	shaderFile.open(filePath);
	if (!shaderFile) {
		std::cout << "Failed to open shader " << filePath << std::endl;
		system("PAUSE");
		exit(1);
	}

	std::string shaderCode = "";
	std::string line = "";
	while (std::getline(shaderFile,line)) {
		//processDirectives(line);
		shaderCode += line + '\n';
	}
	shaderFile.close();
	processDirectives(shaderCode);
	return shaderCode;
}
/**
*Processes preprocessor directives in glsl file
*edits source
**/
void Shader::processDirectives(std::string & source)
{
	processImport(source);
}

/**
*Processes preprocessor import directive
*Syntax: IMPORT_DIRECTIVE path/to/file
*replace directive with filecontent of file
**/
void Shader::processImport(std::string & source)
{
	std::size_t offset = 0;
	std::size_t pos = 0;
	std::vector<std::string> loadedShaderNames;
	while ((pos = source.find(IMPORT_DIRECTIVE,pos+1)) != std::string::npos)
	{
		std::size_t lineEnd = source.find('\n', pos);
		std::size_t len = lineEnd - (pos + IMPORT_DIRECTIVE.length()+1);
		std::string token = source.substr(pos+IMPORT_DIRECTIVE.length()+1, len);
		if (std::find(loadedShaderNames.begin(), loadedShaderNames.end(), token) == loadedShaderNames.end())
		{
			std::string importCode = readFile(SHADER_DIRECTORY+token);
			source.erase(pos, lineEnd - pos);
			source.insert(pos, importCode);
			loadedShaderNames.push_back(token);
		}
		else 
		{
			source.replace(pos, len, "");
		}
		
	}
}

Shader::Shader()
{
}

Shader::Shader(std::string vertexShader, std::string fragmentShader)
{
	this->vertexShader = vertexShader;
	this->fragmentShader = fragmentShader;
	this->handle = loadStandardProgram();
}

Shader::Shader(std::string computeShader)
{
	this->computeShader = computeShader;
	this->handle = loadComputeProgram();
}

void Shader::setUniform(std::string uniform, const glm::vec3& value)
{
	GLint location = getUniformLocation(uniform);
	setUnifrom(location, value);
}

void Shader::setUnifrom(GLint location, const glm::vec3& value)
{
	glUniform3f(location, value.x, value.y, value.z);
}

void Shader::setUniform(std::string uniform, const int value)
{
	GLint location = getUniformLocation(uniform);
	setUniform(location, value);
}

void Shader::setUniform(GLint location, const int value)
{
	glUniform1i(location, value);
}

void Shader::setUniform(std::string uniform, const float value)
{
	GLint location = getUniformLocation(uniform);
	setUniform(location, value);
}

void Shader::setUniform(GLint location, const float value)
{
	glUniform1f(location, value);
}

void Shader::setUniform(std::string uniform, const glm::mat4 & mat)
{
	GLint location = getUniformLocation(uniform);
	setUniform(location, mat);
}

void Shader::setUniform(GLint location, const glm::mat4 & mat)
{
	glUniformMatrix4fv(location, 1, GL_FALSE, &mat[0][0]);
}

void Shader::setUniform(std::string uniform, const glm::mat3 & mat)
{
	GLint location = getUniformLocation(uniform);
	setUniform(location, mat);
}

void Shader::setUniform(GLint location, const glm::mat3 & mat)
{
	glUniformMatrix3fv(location, 1, GL_FALSE, &mat[0][0]);
}

void Shader::use()
{
	glUseProgram(handle);
}

void Shader::unuse()
{
	glUseProgram(0);
}


Shader::~Shader()
{
	GLuint attachedShaders[2];
	GLsizei numberOfShaders;
	glGetAttachedShaders(handle, 2, &numberOfShaders, attachedShaders);
	glDeleteProgram(handle);
	for (int i = 0; i < numberOfShaders; ++i) 
	{
		glDeleteShader(attachedShaders[i]);
	}
	std::cout << "Shader Deleted" << std::endl;
}
