#include "BadModel.h"

#include <iostream>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

BadModel::BadModel(std::string path)
	:
	shader("CelShader.vert", "CelShader.frag")
{
	tinygltf::TinyGLTF loader;
	std::string err;
	std::string warn;

	bool res = loader.LoadASCIIFromFile(&model, &err, &warn, path);
	if (!warn.empty()) {
		std::cout << "WARN: " << warn << std::endl;
	}

	if (!err.empty()) {
		std::cout << "ERR: " << err << std::endl;
	}

	if (!res)
		std::cout << "Failed to load glTF: " << path << std::endl;
	else
		std::cout << "Loaded glTF: " << path << std::endl;

	vao = GenGltfModel(model);

	translationMat = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -3));
	rotationMat = glm::mat4(1.0f);


	view_mat = glm::lookAt(glm::vec3(0.0f, 0.5f, -1.0f), glm::vec3(0, 0, -3), glm::vec3(0, 1, 0));

	projection_mat = glm::perspective(glm::radians(45.0f), 16.0f / 9.0f, 0.01f, 100.0f);
}

void BadModel::Draw()
{
	glBindVertexArray(vao);

	shader.Use();
	shader.SetUniformMat4("mvp", GenerateMVP());

	const tinygltf::Scene& scene = model.scenes[model.defaultScene];
	for (size_t i = 0; i < scene.nodes.size(); i++)
	{
		DrawGltfNodes(model.nodes[scene.nodes[i]]);
	}
	glBindVertexArray(0);
}

unsigned int BadModel::GenGltfModel(tinygltf::Model& model)
{
	std::map<int, unsigned int> vbos;
	unsigned int vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	const tinygltf::Scene& scene = model.scenes[model.defaultScene];
	for (size_t i = 0; i < scene.nodes.size(); ++i) {
		assert((scene.nodes[i] >= 0) && (scene.nodes[i] < model.nodes.size()));
		GenGltfModelNodes(vbos, model, model.nodes[scene.nodes[i]]);
	}

	glBindVertexArray(0);
	// cleanup vbos
	for (int i = 0; i < vbos.size(); ++i) {
		glDeleteBuffers(1, &vbos[i]);
	}

	return vao;
}

void BadModel::GenGltfModelNodes(std::map<int, unsigned int> vbos, tinygltf::Model& model, tinygltf::Node& node)
{
	if ((node.mesh >= 0) && (node.mesh < model.meshes.size())) {
		GenGltfMesh(vbos, model, model.meshes[node.mesh]);
	}

	for (size_t i = 0; i < node.children.size(); i++) {
		assert((node.children[i] >= 0) && (node.children[i] < model.nodes.size()));
		GenGltfModelNodes(vbos, model, model.nodes[node.children[i]]);
	}
}

std::map<int, unsigned int> BadModel::GenGltfMesh(std::map<int, unsigned int> vbos, tinygltf::Model& model, tinygltf::Mesh& mesh)
{
	for (int i = 0; i < model.bufferViews.size(); ++i)
	{
		const tinygltf::BufferView& bufferView = model.bufferViews[i];
		if (bufferView.target == 0)
		{  // TODO impl drawarrays
			std::cout << "WARN: bufferView.target is zero" << std::endl;
			continue;  // Unsupported bufferView.
					   /*
						 From spec2.0 readme:
						 https://github.com/KhronosGroup/glTF/tree/master/specification/2.0
								  ... drawArrays function should be used with a count equal to
						 the count            property of any of the accessors referenced by the
						 attributes            property            (they are all equal for a given
						 primitive).
					   */
		}

		const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];
		std::cout << "bufferview.target " << bufferView.target << std::endl;

		GLuint vbo;
		glGenBuffers(1, &vbo);
		vbos[i] = vbo;
		glBindBuffer(bufferView.target, vbo);

		std::cout << "buffer.data.size = " << buffer.data.size()
			<< ", bufferview.byteOffset = " << bufferView.byteOffset
			<< std::endl;

		glBufferData(bufferView.target, bufferView.byteLength,
			&buffer.data.at(0) + bufferView.byteOffset, GL_STATIC_DRAW);
	}

	for (size_t i = 0; i < mesh.primitives.size(); ++i) {
		tinygltf::Primitive primitive = mesh.primitives[i];
		tinygltf::Accessor indexAccessor = model.accessors[primitive.indices];

		for (auto& attrib : primitive.attributes) {
			tinygltf::Accessor accessor = model.accessors[attrib.second];
			int byteStride = accessor.ByteStride(model.bufferViews[accessor.bufferView]);
			glBindBuffer(GL_ARRAY_BUFFER, vbos[accessor.bufferView]);

			int size = 1;
			if (accessor.type != TINYGLTF_TYPE_SCALAR) {
				size = accessor.type;
			}

			int vaa = -1;
			if (attrib.first.compare("POSITION") == 0) vaa = 0;
			if (attrib.first.compare("NORMAL") == 0) vaa = 1;
			if (attrib.first.compare("TEXCOORD_0") == 0) vaa = 2;
			if (vaa > -1) {
				glEnableVertexAttribArray(vaa);
				glVertexAttribPointer(vaa, size, accessor.componentType,
					accessor.normalized ? GL_TRUE : GL_FALSE,
					byteStride, BUFFER_OFFSET(accessor.byteOffset));
			}
			else
				std::cout << "vaa missing: " << attrib.first << std::endl;
		}

		if (model.textures.size() > 0) {
			// fixme: Use material's baseColor
			tinygltf::Texture& tex = model.textures[0];

			if (tex.source > -1) {

				GLuint texid;
				glGenTextures(1, &texid);

				tinygltf::Image& image = model.images[tex.source];

				glBindTexture(GL_TEXTURE_2D, texid);
				glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

				GLenum format = GL_RGBA;

				if (image.component == 1) {
					format = GL_RED;
				}
				else if (image.component == 2) {
					format = GL_RG;
				}
				else if (image.component == 3) {
					format = GL_RGB;
				}
				else {
					// ???
				}

				GLenum type = GL_UNSIGNED_BYTE;
				if (image.bits == 8) {
					// ok
				}
				else if (image.bits == 16) {
					type = GL_UNSIGNED_SHORT;
				}
				else {
					// ???
				}

				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height, 0,
					format, type, &image.image.at(0));
			}
		}
	}

	return vbos;
}

void BadModel::DrawGltfNodes(tinygltf::Node& node)
{
	if ((node.mesh >= 0) && (node.mesh < model.meshes.size())) {
		tinygltf::Mesh& mesh = model.meshes[node.mesh];
		for (size_t i = 0; i < mesh.primitives.size(); ++i) {
			tinygltf::Primitive primitive = mesh.primitives[i];
			tinygltf::Accessor indexAccessor = model.accessors[primitive.indices];

			glDrawElements(primitive.mode, (GLsizei)indexAccessor.count,
				indexAccessor.componentType,
				BUFFER_OFFSET(indexAccessor.byteOffset));
		}
	}
	for (size_t i = 0; i < node.children.size(); i++) {
		DrawGltfNodes(model.nodes[node.children[i]]);
	}
}

glm::mat4 BadModel::GenerateMVP()
{
	rotationMat = glm::rotate(rotationMat, glm::radians(0.8f), glm::vec3(0, 1, 0));
	glm::mat4 modelMat = translationMat * rotationMat;
	shader.SetUniformMat4("model", modelMat);
	return projection_mat * view_mat * modelMat;
}

