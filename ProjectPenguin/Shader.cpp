#include "Shader.h"

#include <glad/glad.h>

#include "GlGetError.h"

Shader::Shader(std::string vertexName, std::string fragmentName)
	:
	Shader(vertexName, fragmentName, "")
{
}

Shader::Shader(std::string vertexName, std::string fragmentName, std::string geometryName)
{
	bool useGeometryShader = !geometryName.empty();
	GL_ERROR_CHECK();

	std::string vertexPath = "Shaders/";
	std::string fragmentPath = "Shaders/";
	std::string geometryPath = "Shaders/";

	vertexPath.append(vertexName);
	fragmentPath.append(fragmentName);
	geometryPath.append(geometryName);

	//Load shader code from file
	std::string vertexCode = FromFile(vertexPath);
	std::string fragmentCode = FromFile(fragmentPath);
	std::string geometryCode;
	if (useGeometryShader)
	{
		geometryCode = FromFile(geometryPath);
	}

	//Compile shaders
	unsigned int vertexShader = CreateShader(vertexName, vertexCode.c_str(), GL_VERTEX_SHADER);
	unsigned int fragmentShader = CreateShader(fragmentName, fragmentCode.c_str(), GL_FRAGMENT_SHADER);
	unsigned int geometryShader;
	if (useGeometryShader)
	{
		geometryShader = CreateShader(geometryName, geometryCode.c_str(), GL_GEOMETRY_SHADER);
	}
	GL_ERROR_CHECK();

	//Link shaders
	shaderProgram = glCreateProgram();
	assert(shaderProgram > 0);
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	if (useGeometryShader)
	{
		glAttachShader(shaderProgram, geometryShader);
	}
	glLinkProgram(shaderProgram);
	{
		int  success;
		char infoLog[512];
		glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::LINKING_FAILED\n" << infoLog << std::endl;
			std::string errorMessage = "Could not link shaders ";
			errorMessage.append(vertexName);
			errorMessage.append(" and ");
			errorMessage.append(fragmentName);
			if (useGeometryShader)
			{
				errorMessage.append("and");
				errorMessage.append(geometryName);
			}
			errorMessage.append("\n\ninfoLog:\n");
			errorMessage.append(infoLog);
			throw std::exception(errorMessage.c_str());
		}
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	if (useGeometryShader)
	{
		glDeleteShader(geometryShader);
	}
	GL_ERROR_CHECK();
}

Shader::~Shader()
{
	if (shaderProgram > 0)
	{
		glDeleteProgram(shaderProgram);
	}
}

Shader::Shader(Shader&& rhs) noexcept
	:
	shaderProgram(rhs.shaderProgram)
{
	rhs.shaderProgram = 0;
}

unsigned int Shader::Get() const
{
	return shaderProgram;
}

void Shader::Use() const
{
	GL_ERROR_CHECK();
	glUseProgram(shaderProgram);
	GL_ERROR_CHECK();
}

void Shader::SetUniformBool(const std::string& name, bool value) const
{
	glUniform1i(glGetUniformLocation(shaderProgram, name.c_str()), (int)value);
}

void Shader::SetUniformInt(const std::string& name, int value) const
{
	glUniform1i(glGetUniformLocation(shaderProgram, name.c_str()), value);
}

void Shader::SetUniformFloat(const std::string& name, float value) const
{
	glUniform1f(glGetUniformLocation(shaderProgram, name.c_str()), value);
}

void Shader::SetUniformVec2(const std::string& name, const glm::vec2& value) const
{
	glUniform2fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, &value[0]);
}

void Shader::SetUniformVec2(const std::string& name, float x, float y) const
{
	glUniform2f(glGetUniformLocation(shaderProgram, name.c_str()), x, y);
}

void Shader::SetUniformVec3(const std::string& name, const glm::vec3& value) const
{
	glUniform3fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, &value[0]);
}

void Shader::SetUniformVec3(const std::string& name, float x, float y, float z) const
{
	glUniform3f(glGetUniformLocation(shaderProgram, name.c_str()), x, y, z);
}

void Shader::SetUniformVec4(const std::string& name, const glm::vec4& value) const
{
	glUniform4fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, &value[0]);
}

void Shader::SetUniformVec4(const std::string& name, float x, float y, float z, float w) const
{
	glUniform4f(glGetUniformLocation(shaderProgram, name.c_str()), x, y, z, w);
}

void Shader::SetUniformMat2(const std::string& name, const glm::mat2& mat) const
{
	glUniformMatrix2fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::SetUniformMat3(const std::string& name, const glm::mat3& mat) const
{
	glUniformMatrix3fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::SetUniformMat4(const std::string& name, const glm::mat4& mat) const
{
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::SetUniformMat4Array(const std::string& name, const std::vector<glm::mat4>& values) const
{
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, name.c_str()), (GLsizei)values.size(), GL_FALSE, &values.front()[0][0]);
}

void Shader::SetUniformVec3Array(const std::string& name, const std::vector<glm::vec3>& values) const
{
	glUniform3fv(glGetUniformLocation(shaderProgram, name.c_str()), (GLsizei)values.size(), &values.front()[0]);
}

std::string Shader::FromFile(std::string path)
{
	std::ifstream file;
	file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	std::stringstream content;
	try
	{
		//Open file
		file.open(path);
		//Read file into stream
		content << file.rdbuf();
		//Close file
		file.close();
	}
	catch (std::ifstream::failure& e)
	{
		std::string errorMessage = "Failed to load shader: ";
		errorMessage.append(path);
		errorMessage.append("\n\n");
		errorMessage.append(e.what());
		throw std::exception(errorMessage.c_str());
	}
	//return contents as string
	return content.str();
}

unsigned int Shader::CreateShader(std::string name, const char* source, unsigned int type)
{
	//Create and compile shader
	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, &source, nullptr);
	glCompileShader(shader);
	int  success;
	char infoLog[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(shader, 512, nullptr, infoLog);
		std::string errorMessage = name;
		errorMessage.append(" could not be compiled:\n");
		errorMessage.append(infoLog);
		throw std::exception(errorMessage.c_str());
	}
	return shader;
}
