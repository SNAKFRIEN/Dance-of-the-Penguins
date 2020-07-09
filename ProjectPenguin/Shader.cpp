#include "Shader.h"

Shader::Shader(std::string vertexName, std::string fragmentName)
{
	std::string vertexPath = "Shaders/";
	std::string fragmentPath = "Shaders/";

	vertexPath.append(vertexName);
	fragmentPath.append(fragmentName);

	//Load shader code from file
	std::string vertexCode = FromFile(vertexPath);
	std::string fragmentCode = FromFile(fragmentPath);

	//Compile shaders
	unsigned int vertexShader = CreateShader(vertexCode.c_str(), GL_VERTEX_SHADER);
	unsigned int fragmentShader = CreateShader(fragmentCode.c_str(), GL_FRAGMENT_SHADER);

	//Link shaders
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	{
		int  success;
		char infoLog[512];
		glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::LINKING_FAILED\n" << infoLog << std::endl;
			throw;
		}
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

unsigned int Shader::Get() const
{
	return shaderProgram;
}

void Shader::Use() const
{
	glUseProgram(shaderProgram);
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
	catch (std::ifstream::failure e)
	{
		std::string errorMessage = "Failed to load shader: ";
		errorMessage.append(path);
		throw std::exception(errorMessage.c_str());
	}
	//return contents as string
	return content.str();
}

unsigned int Shader::CreateShader(const char* source, unsigned int type)
{
	//Create and compile shader
	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, &source, nullptr);
	glCompileShader(shader);
	{
		int  success;
		char infoLog[512];
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 512, nullptr, infoLog);
			std::cout << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
			throw;
		}
	}
	return shader;
}
