#include "Model.h"

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

#include "Camera.h"
#include "Light.h"

//REMOVE use the proper error check method
#define GL_ERROR_CHECK();\
{\
	int error = glGetError();\
	if (error != GL_NO_ERROR)\
	{\
		std::stringstream errorMessage;\
		errorMessage << "GL error: 0x" << std::hex << error << "\n" << __FILE__ << " " << __LINE__;\
		throw std::exception(errorMessage.str().c_str());\
	}\
}

//Static members
std::unordered_map<std::string, Model::ModelData> Model::existingModels;

Model::Model(std::string name, const glm::mat4& ownerTransform, std::string vertexShader, std::string fragShader)
	:
	ownerTransform(ownerTransform),
	modelData(ConstructModelData(name, vertexShader, fragShader))
{

}

void Model::Preload(std::string name, std::string vertexShader, std::string fragShader)
{
	ConstructModelData(name, vertexShader, fragShader);
}

void Model::AddToRenderQueue(Camera& camera)
{
	//Add model transform and MVP to renderqueue
	modelData.renderQueue.push_back(std::make_pair(ownerTransform, camera.GetVPMatrix() * ownerTransform));
}

void Model::DrawAllInstances(const Light& light)
{
	for (std::pair<const std::string, ModelData>& element : existingModels)
	{
		ModelData& model = element.second;
		
		//Bind shader
		model.shader->Use();
		
		GL_ERROR_CHECK();

		//Bind texture
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, model.texture);
		model.shader->SetUniformInt("tex", 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, light.GetShadowMapTexture());
		model.shader->SetUniformInt("shadowMap", 1);

		GL_ERROR_CHECK();

		//Bind vao
		glBindVertexArray(model.vao);

		GL_ERROR_CHECK()

		//Draw instances
		for (auto& instance : model.renderQueue)
		{
			const auto modelTransform = instance.first;
			const auto transform = instance.second;

			model.shader->SetUniformMat4("model", modelTransform);
			model.shader->SetUniformMat4("mvp", transform);
			model.shader->SetUniformMat4("lightTransform", light.GetLightTransform());

			GL_ERROR_CHECK()

			glDrawElements(GL_TRIANGLES, (GLsizei)model.nIndices, GL_UNSIGNED_SHORT, 0);
			
			GL_ERROR_CHECK()
		}

		//Unbind vao
		glBindVertexArray(0);

		//Clear renderqueue
		model.renderQueue.clear();

		GL_ERROR_CHECK()
	}
}

void Model::DrawShadows(const Light& light)
{
	for (std::pair<const std::string, ModelData>& element : existingModels)
	{
		ModelData& model = element.second;

		//Bind vao
		glBindVertexArray(model.vao);

		GL_ERROR_CHECK();

		//Draw instances
		for (auto& instance : model.renderQueue)
		{
			const auto modelTransform = instance.first;
			const auto transform = light.GetLightTransform() * modelTransform;

			light.GetNonAnimationShader().SetUniformMat4("mvp", transform);

			GL_ERROR_CHECK();

			glDrawElements(GL_TRIANGLES, (GLsizei)model.nIndices, GL_UNSIGNED_SHORT, 0);

			GL_ERROR_CHECK();
		}

		//Unbind vao
		glBindVertexArray(0);

		GL_ERROR_CHECK()
	}
}

Model::ModelData& Model::ConstructModelData(std::string name, std::string vertexShader, std::string fragShader)
{
	//Check if model has been previously loaded
	//WARNING: THIS MAKES IT SO THAT ALL INSTANCES OF THE SAME MODEL USE THE SAME SHADER!
	if (existingModels.count(name) == 0)
	{
		//-------------------------Step 0: Add model data-------------------------------------------------
		auto& newModelData = existingModels[name];

		//-------------------------Step 1: Make the shader-------------------------------------------------
		newModelData.shader = std::make_unique<Shader>(vertexShader, fragShader);

		//-------------------------Step 2: Load the model using tinyGLTF-------------------------------------------------
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
				throw std::invalid_argument(errorMessage.c_str());
			}
			if (!warn.empty())
			{
				std::string errorMessage = "Failed to load model: ";
				errorMessage.append(name);
				errorMessage.append("\n\n");
				errorMessage.append("Received the following warning(s): ");
				errorMessage.append(err);
				throw std::invalid_argument(errorMessage.c_str());
			}
		}

		//-------------------------Step 3: Step down the gltf hierarchy to get to a primitive-------------------------------------------------
		//Step down the GLTF structure to reach the primitive (REPLACE: test just getting data.primitives[0] to skip this step)
		const tinygltf::Scene& scene = data.scenes[0];
		const tinygltf::Node& node = data.nodes[scene.nodes[0]];
		const tinygltf::Mesh& mesh = data.meshes[0];
		const tinygltf::Primitive& primitiveData = mesh.primitives[0];
		GL_ERROR_CHECK()


		//-------------------------Step 4: Set up vao,vbo,ebo and set up vertex attrib pointers-------------------------------------------------
		//Generate VAO, VBO and EBO
		glGenVertexArrays(1, &newModelData.vao);
		glBindVertexArray(newModelData.vao);

		unsigned int ebo;
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
		newModelData.nIndices = indexAccessor.count;

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


		//-------------------------Step 4: Set up the texture-------------------------------------------------
		//Gain access to the gltf data
		if (data.materials.empty())
		{
			std::string errorMessage;
			errorMessage.append("The model \"");
			errorMessage.append(name);
			errorMessage.append("\" could not be loaded, because it doesn't have any materials");
			throw std::exception(errorMessage.c_str());
		}
		const tinygltf::Material& material = data.materials[primitiveData.material];
		if (data.textures.empty())
		{
			std::string errorMessage;
			errorMessage.append("The model \"");
			errorMessage.append(name);
			errorMessage.append("\" could not be loaded, because it doesn't have any textures");
			throw std::exception(errorMessage.c_str());
		}
		tinygltf::Texture& textureData = data.textures[material.pbrMetallicRoughness.baseColorTexture.index];
		tinygltf::Image& image = data.images[textureData.source];

		//Figure out format
		GLenum format;
		switch (image.component)
		{
		case 1:
			format = GL_RED;
			break;
		case 2:
			format = GL_RG;
			break;
		case 3:
			format = GL_RGB;
			break;
		case 4:
			format = GL_RGBA;
			break;
		default:
			std::string errorMessage;
			errorMessage.append("The texture for ");
			errorMessage.append(name);
			errorMessage.append(" could not be loaded, because it had an unsupported format: ");
			errorMessage.append(std::to_string(image.component));
			errorMessage.append(" components");
			throw std::exception(errorMessage.c_str());
		}

		//Figure out the type
		GLenum type;
		switch (image.bits)
		{
		case 8:
			type = GL_UNSIGNED_BYTE;
			break;
		case 16:
			type = GL_UNSIGNED_SHORT;
			break;
		}

		//Generate texture
		glGenTextures(1, &newModelData.texture);
		glBindTexture(GL_TEXTURE_2D, newModelData.texture);

		//Set texture settings
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		//Load data into texture (REPLACE: might want to use GL_RGBA instead of GL_RGB to support transparent textures, or vice versa to save space)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.width, image.height, 0, format, type, &image.image.at(0));

		GL_ERROR_CHECK()
	}

	return existingModels.at(name);
}