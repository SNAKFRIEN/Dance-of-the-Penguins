#include "GLTFLoader.h"

GLTFLoader::GLTFLoader(std::string name)
	:
	shader("CelShader.vert", "CelShader.frag")
{
	//-------------------------Step 1: Load the model using tinyGLTF-------------------------------------------------
	//Import the model and check errors
	tinygltf::TinyGLTF loader;
	tinygltf::Model data;
	{
		std::string err;
		std::string warn;
		//REPLACE: path should be automatically adjusted to lead to model folder so that you can simply supply the name of the file rather than the entire path
		loader.LoadASCIIFromFile(&data, &err, &warn, name);


		if (!err.empty())
		{
			std::string errorMessage = "Failed to load model: ";
			errorMessage.append(name);
			errorMessage.append("\n\n");
			errorMessage.append("Received the following error: ");
			errorMessage.append(err);
			throw std::exception(errorMessage.c_str());
		}
		if (!warn.empty())
		{
			std::string errorMessage = "Failed to load model: ";
			errorMessage.append(name);
			errorMessage.append("\n\n");
			errorMessage.append("Received the following warning: ");
			errorMessage.append(err);
			throw std::exception(errorMessage.c_str());
		}
	}



	//-------------------------Step 2: Step down the gltf hierarchy to get to a primitive-------------------------------------------------
	//Step down the GLTF structure to reach the primitive (REPLACE: test just getting data.primitives[0] to skip this step)
	const tinygltf::Scene& scene = data.scenes[0];
	const tinygltf::Node& node = data.nodes[scene.nodes[0]];
	const tinygltf::Mesh& mesh = data.meshes[node.mesh];
	const tinygltf::Primitive& primitiveData = mesh.primitives[0];

	

	//-------------------------Step 3: Set up vao,vbo,ebo and set up vertex attrib pointers-------------------------------------------------
	//Generate VAO, VBO and EBO
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);

	glGenBuffers(1, &ebo);

	//Rettrieve attribute data and fill VBO, set up
	const tinygltf::Accessor& attribAccessor = data.accessors[primitiveData.attributes.begin()->second];
	const tinygltf::BufferView& attribBufferView = data.bufferViews[attribAccessor.bufferView];
	const tinygltf::Buffer& attribBuffer = data.buffers[attribBufferView.buffer];
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, attribBufferView.byteLength, &attribBuffer.data.at(0) + attribBufferView.byteOffset, GL_STATIC_DRAW);
	
	//Retrieve index data and fill EBO
	const tinygltf::Accessor& indexAccessor = data.accessors[primitiveData.indices];
	const tinygltf::BufferView& indexBufferView = data.bufferViews[indexAccessor.bufferView];
	const tinygltf::Buffer& indexBuffer = data.buffers[indexBufferView.buffer];
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBufferView.byteLength, &indexBuffer.data.at(0) + indexBufferView.byteOffset, GL_STATIC_DRAW);

	//Set up vertex attrib pointers
	//REPLACE: Check if it's actually necessary to bind the vbo at this point
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	for (auto& attrib : primitiveData.attributes)
	{
		const tinygltf::Accessor& accessor = data.accessors[attrib.second];
		int byteStride = accessor.ByteStride(data.bufferViews[accessor.bufferView]);

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
			glEnableVertexAttribArray(vertexPointer);
			glVertexAttribPointer(vertexPointer,
				size,
				accessor.componentType,
				accessor.normalized ? GL_TRUE : GL_FALSE,
				byteStride,
				(char*)0 + accessor.byteOffset);
		}
		else
		{
			std::string errorMessage;
			errorMessage.append("The model \"");
			errorMessage.append(name);
			errorMessage.append("\" could not be loaded, its vertices do not have a ");
			errorMessage.append(attrib.first);
			throw std::exception(errorMessage.c_str());
		}
	}



	//-------------------------Step 4: Set up the texture-------------------------------------------------
	//Gain access to the gltf data
	const tinygltf::Material& material = data.materials[primitiveData.material];
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
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	//Set texture settings
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//Load data into texture (REPLACE: might want to use GL_RGBA instead of GL_RGB to support transparent textures, or vice versa to save space)
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.width, image.height, 0, format, type, &image.image.at(0));
}

