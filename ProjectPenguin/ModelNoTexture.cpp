#include "ModelNoTexture.h"

#include "glad/glad.h"
#include <glm/gtc/matrix_transform.hpp>

#include "Camera.h"

#define GL_ERROR_CHECK();\
{\
	int error = glGetError();\
	if (error != GL_NO_ERROR)\
	{\
		std::stringstream errorMessage;\
		errorMessage << "GL error: 0x" << std::hex << error;\
		throw std::exception(errorMessage.str().c_str());\
	}\
}

ModelNoTexture::ModelNoTexture(std::string name, const glm::mat4& ownerTransform)
	:
	shader("NoTextureShader.vert", "NoTextureShader.frag"),
	ownerTransform(ownerTransform)
{
	//-------------------------Step 1: Load the model using tinyGLTF-------------------------------------------------
	//Import the model and check errors
	tinygltf::TinyGLTF loader;
	tinygltf::Model data;
	{
		std::string err;
		std::string warn;
		//REPLACE: path should be automatically adjusted to lead to model folder so that you can simply supply the name of the file rather than the entire path
		std::string path = "Models/";
		path.append(name);
		loader.LoadASCIIFromFile(&data, &err, &warn, path);

		if (!err.empty())
		{
			std::string errorMessage = "Failed to load model: ";
			errorMessage.append(name);
			errorMessage.append("\n\n");
			errorMessage.append("Received the following error(s): ");
			errorMessage.append(err);
			throw std::exception(errorMessage.c_str());
		}
		if (!warn.empty())
		{
			std::string errorMessage = "Failed to load model: ";
			errorMessage.append(name);
			errorMessage.append("\n\n");
			errorMessage.append("Received the following warning(s): ");
			errorMessage.append(err);
			throw std::exception(errorMessage.c_str());
		}
	}

	//-------------------------Step 2: Step down the gltf hierarchy to get to a primitive-------------------------------------------------
	//Step down the GLTF structure to reach the primitive (REPLACE: test just getting data.primitives[0] to skip this step)
	const tinygltf::Scene& scene = data.scenes[0];
	const tinygltf::Node& node = data.nodes[scene.nodes[0]];
	const tinygltf::Mesh& mesh = data.meshes[0];
	const tinygltf::Primitive& primitiveData = mesh.primitives[0];
	GL_ERROR_CHECK()


	//-------------------------Step 3: Set up vao,vbo,ebo and set up vertex attrib pointers-------------------------------------------------
	//Generate VAO, VBO and EBO
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	//glGenBuffers(1, &vbo);

	unsigned ebo;
	glGenBuffers(1, &ebo);

	//Rettrieve attribute data and fill VBO, set up
	std::map<int, unsigned int> vbos;
	for (int i = 0; i < data.bufferViews.size(); ++i)
	{
		const tinygltf::BufferView& bufferView = data.bufferViews[i];
		if (bufferView.target == 0)
		{  // TODO impl drawarrays
			std::cout << "WARN: bufferView.target is zero" << std::endl;
			continue;
		}

		const tinygltf::Buffer& buffer = data.buffers[bufferView.buffer];
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

	//std::vector<unsigned int> vbos;

	/*
	for (auto& attrib : primitiveData.attributes)
	{
		const tinygltf::Accessor& attribAccessor = data.accessors[attrib.second];
		const tinygltf::BufferView& attribBufferView = data.bufferViews[attribAccessor.bufferView];
		std::cout << "bufferView.target " << attribBufferView.target << std::endl;

		const tinygltf::Buffer& attribBuffer = data.buffers[attribBufferView.buffer];
		GLuint temp;
		glGenBuffers(1, &temp);
		glBindBuffer(GL_ARRAY_BUFFER, temp);
		glBufferData(GL_ARRAY_BUFFER, attribBufferView.byteLength, &attribBuffer.data.at(0) + attribBufferView.byteOffset, GL_STATIC_DRAW);
		vbos.push_back(temp);
	}
	*/

	//Retrieve index data and fill EBO and save nIndices
	const tinygltf::Accessor& indexAccessor = data.accessors[primitiveData.indices];
	const tinygltf::BufferView& indexBufferView = data.bufferViews[indexAccessor.bufferView];
	const tinygltf::Buffer& indexBuffer = data.buffers[indexBufferView.buffer];
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBufferView.byteLength, &indexBuffer.data.at(0) + indexBufferView.byteOffset, GL_STATIC_DRAW);
	nIndices = indexAccessor.count;

	//Set up vertex attrib pointers
	for (auto& attrib : primitiveData.attributes)
	{
		const tinygltf::Accessor& accessor = data.accessors[attrib.second];
		int byteStride = accessor.ByteStride(data.bufferViews[accessor.bufferView]);
		glBindBuffer(GL_ARRAY_BUFFER, vbos[accessor.bufferView]);

		int size = 1;
		if (accessor.type != TINYGLTF_TYPE_SCALAR)
		{
			size = accessor.type;
		}

		int vertexPointer = -1;
		if (attrib.first.compare("POSITION") == 0) vertexPointer = 0;
		if (attrib.first.compare("NORMAL") == 0) vertexPointer = 1;
		if (attrib.first.compare("TEXCOORD_0") == 0) vertexPointer = 2;
		if (vertexPointer > -1)
		{
			glVertexAttribPointer(vertexPointer,
				size,
				accessor.componentType,
				accessor.normalized ? GL_TRUE : GL_FALSE,
				byteStride,
				(char*)0 + accessor.byteOffset);
			glEnableVertexAttribArray(vertexPointer);
		}
		else
		{
			std::string errorMessage;
			errorMessage.append("The model \"");
			errorMessage.append(name);
			errorMessage.append("\" could not be loaded, the following vertex attribute is not supported: ");
			errorMessage.append(attrib.first);
			throw std::exception(errorMessage.c_str());
		}
	}
	GL_ERROR_CHECK();
}

void ModelNoTexture::Draw(Camera& camera)
{
	//Correct model matrix
	glm::mat4 model = glm::scale(ownerTransform, glm::vec3(-1.0f, 1.0f, -1.0f));

	//Calculate MVP
	const auto transform = camera.GetVPMatrix() * glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	GL_ERROR_CHECK()

	//Bind shader and uniforms
	shader.Use();
	shader.SetUniformMat4("model", model);
	shader.SetUniformMat4("mvp", transform);

	GL_ERROR_CHECK()

	//Bind vao and draw (uses ebo)
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, (GLsizei)nIndices, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);

	GL_ERROR_CHECK()
}
