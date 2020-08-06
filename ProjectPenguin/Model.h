#pragma once

#define TINYGLTF_NO_STB_IMAGE_WRITE
#include "tiny_gltf.h"
#include "glm/glm.hpp"

#include "Shader.h"

class Camera;

class Model
{
public:
	Model(std::string name,
		const glm::mat4& ownerTransform,
		std::string vertexShader = "CelShader.vert",
		std::string fragShader = "CelShader.frag");
	~Model();
	Model(const Model&) = delete;
	Model operator=(const Model&) = delete;
	Model(Model&& rhs) = delete;
	Model operator=(Model&& rhs) = delete;

	void Draw(Camera& camera);
	static constexpr int test = 5;
private:
	//Geometry
	unsigned int vao = 0;
	std::map<int, unsigned int> vbos;
	unsigned int ebo = 0;
	size_t nIndices = 0;

	//Shader
	Shader shader;

	//Texture
	unsigned int texture = 0;	//only supports models with single textures for now

	//Reference to owner transform
	const glm::mat4& ownerTransform;
};