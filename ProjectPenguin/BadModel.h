#pragma once

#define TINYGLTF_NO_STB_IMAGE_WRITE
#include "tiny_gltf.h"
#include <glm/glm.hpp>

#include "Shader.h"

class BadModel
{
public:
	BadModel(std::string path);
	void Draw();
private:
	unsigned int GenGltfModel(tinygltf::Model& model);
	void GenGltfModelNodes(std::map<int, unsigned int> vbos, tinygltf::Model& model, tinygltf::Node& node);
	std::map<int, unsigned int> GenGltfMesh(std::map<int, unsigned int> vbos, tinygltf::Model& model, tinygltf::Mesh& mesh);

	void DrawGltfNodes(tinygltf::Node& node);

	glm::mat4 GenerateMVP();
private:
	unsigned int vao;

	glm::mat4 translationMat;
	glm::mat4 rotationMat;

	glm::mat4 view_mat;

	glm::mat4 projection_mat;

	Shader shader;

	tinygltf::Model model;
};