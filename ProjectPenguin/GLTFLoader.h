#pragma once

#define TINYGLTF_NO_STB_IMAGE_WRITE
#include "tiny_gltf.h"
#include <glm/glm.hpp>

#include "Shader.h"

struct BufferData
{

};

class GLTFLoader
{
public:
	GLTFLoader(std::string name);
private:
	unsigned int vao = 0;
	unsigned int vbo = 0;
	unsigned int ebo = 0;
	Shader shader;
	unsigned int texture = 0;	//only supports models with single textures for now
};