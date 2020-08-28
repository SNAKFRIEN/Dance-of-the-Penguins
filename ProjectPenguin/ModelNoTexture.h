#pragma once

//REMOVE when game is finished

#define TINYGLTF_NO_STB_IMAGE_WRITE
#include "tiny_gltf.h"
#include <glm/glm.hpp>

#include "Shader.h"

class Camera;

class ModelNoTexture
{
public:
	ModelNoTexture(std::string name, const glm::mat4& ownerTransform);
	void Draw(Camera& camera);
private:
	//Geometry
	unsigned int vao = 0;
	size_t nIndices = 0;

	//Shader
	Shader shader;

	//Reference to owner transform
	const glm::mat4& ownerTransform;
};