#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:
	Shader(std::string vertexPath, std::string fragmentPath);
	
	unsigned int Get() const;

	void Use() const;

	void SetUniformBool(const std::string& name, bool value) const;
	void SetUniformInt(const std::string& name, int value) const;
	void SetUniformFloat(const std::string& name, float value) const;
	void SetUniformVec2(const std::string& name, const glm::vec2& value) const;
	void SetUniformVec2(const std::string& name, float x, float y) const;
	void SetUniformVec3(const std::string& name, const glm::vec3& value) const;
	void SetUniformVec3(const std::string& name, float x, float y, float z) const;
	void SetUniformVec4(const std::string& name, const glm::vec4& value) const;
	void SetUniformVec4(const std::string& name, float x, float y, float z, float w) const;
	void SetUniformMat2(const std::string& name, const glm::mat2& mat) const;
	void SetUniformMat3(const std::string& name, const glm::mat3& mat) const;
	void SetUniformMat4(const std::string& name, const glm::mat4& mat) const;
private:
	std::string FromFile(std::string path);
	unsigned int CreateShader(const char* source, unsigned int type);
private:
	unsigned int shaderProgram;
};