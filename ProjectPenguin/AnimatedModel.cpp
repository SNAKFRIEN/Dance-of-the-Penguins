#include "AnimatedModel.h"

#include <glm/gtc/matrix_transform.hpp>

#include "Camera.h"
#include "GLTFData.h"

//REMOVE use the proper error check method
#define GL_ERROR_CHECK()\
{\
	int error = glGetError();\
	if (error != GL_NO_ERROR)\
	{\
		std::stringstream errorMessage;\
		errorMessage << "GL error: 0x" << std::hex << error;\
		throw std::exception(errorMessage.str().c_str());\
	}\
}

//Static members
std::unordered_map<std::string, AnimatedModel::ModelData> AnimatedModel::existingModels;

AnimatedModel::AnimatedModel(std::string name, const glm::mat4& ownerTransform, std::string vertexShader, std::string fragShader)
	:
	ownerTransform(ownerTransform),
	modelData(ConstructModelData(name, vertexShader, fragShader))
{
}

void AnimatedModel::Update(float dt)
{
	animationTime += dt;
	if (animationTime > modelData.animations[currentAnimation].duration)
	{
		animationTime = fmod(animationTime, modelData.animations[currentAnimation].duration);
	}

	//Find previous and next frame
	const std::vector<KeyFrame>& frames = modelData.animations[currentAnimation].frames;
	KeyFrame prevFrame = frames[0];
	KeyFrame nextFrame = frames[1];
	for (int i = 0; i < frames.size(); i++)
	{
		if (frames[i].timeStamp < animationTime)
		{
			prevFrame = frames[i];
			nextFrame = frames[i + 1];
		}
	}

	//Calculate current pose
	float progression = KeyFrame::CalculateProgression(prevFrame, nextFrame, animationTime);

	//Interpolate between the poses to get current pose
	std::vector<glm::mat4> currentPose = KeyFrame::Interpolate(prevFrame, nextFrame, progression);
	
	//Update transforms
	for (Joint* j : modelData.rootJoints)
	{
		ApplyPoseToJointsRecursively(currentPose, *j, glm::mat4(1));
	}

	//Store pose so that it can be sent to GPU when it's time to draw
	pose = GetJointTransforms();
}

void AnimatedModel::AddToRenderQueue(Camera& camera)
{
	//Store model matrix
	const auto modelTransform = ownerTransform;

	//Calculate MVP
	const auto transform = camera.GetVPMatrix() * modelTransform;

	//Gather joint transforms
	std::vector<glm::mat4> jointTransforms = pose;

	modelData.renderQueue.push_back(std::make_tuple(modelTransform, transform, jointTransforms));
}

void AnimatedModel::DrawAllInstances()
{
	for (std::pair<const std::string, ModelData>& element : existingModels)
	{
		ModelData& model = element.second;

		//Bind texture
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, model.texture);

		GL_ERROR_CHECK();

		//Bind shader
		model.shader->Use();

		GL_ERROR_CHECK();

		//Bind vao
		glBindVertexArray(model.vao);

		//Draw instances
		for (auto& instance : model.renderQueue)
		{
			glm::mat4 modelTransform;
			glm::mat4 transform;
			std::vector<glm::mat4> jointTransforms;
			std::tie(modelTransform, transform, jointTransforms) = instance;

			model.shader->SetUniformMat4("model", modelTransform);
			model.shader->SetUniformMat4("mvp", transform);
			model.shader->SetUniformMat4Array("jointTransforms", jointTransforms);

			glDrawElements(GL_TRIANGLES, (GLsizei)model.nIndices, GL_UNSIGNED_SHORT, 0);
		}

		//Unbind vao
		glBindVertexArray(0);

		//Clear renderqueue
		model.renderQueue.clear();

		GL_ERROR_CHECK();
	}
}

void AnimatedModel::SetAnimation(std::string name)
{
	assert(modelData.animations.count(name) > 0);	//Make sure animation exists
	currentAnimation = name;
}

int AnimatedModel::GetJointIndex(std::string jointName) const
{
	for (Joint j : modelData.joints)
	{
		if (j.name == jointName)
		{
			return j.id;
		}
	}
	assert(false);	//jointName could not be found
	return -1;
}

const std::vector<glm::mat4>& AnimatedModel::GetPose() const
{
	return pose;
}

const glm::mat4& AnimatedModel::GetTransform() const
{
	return ownerTransform;
}

AnimatedModel::ModelData& AnimatedModel::ConstructModelData(std::string name, std::string vertexShader, std::string fragShader)
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

		//-------------------------Step 3: Step down the gltf hierarchy to get to a primitive-------------------------------------------------
		//Step down the GLTF structure to reach the primitive (REPLACE: test just getting data.primitives[0] to skip this step)
		const tinygltf::Scene& scene = data.scenes[0];
		const tinygltf::Node& node = data.nodes[scene.nodes[0]];
		const tinygltf::Mesh& mesh = data.meshes[0];
		const tinygltf::Primitive& primitiveData = mesh.primitives[0];
		const tinygltf::Skin& skin = data.skins[0];
		GL_ERROR_CHECK();


		//-------------------------Step 4: Set up vao,vbo,ebo and set up vertex attrib pointers-------------------------------------------------
		//Generate VAO, VBO and EBO
		glGenVertexArrays(1, &newModelData.vao);
		glBindVertexArray(newModelData.vao);

		unsigned int ebo;
		glGenBuffers(1, &ebo);

		//Rettrieve attribute data and fill VBO
		std::map<int, unsigned int> vbos;
		for (int i = 0; i < data.bufferViews.size(); ++i)
		{
			const tinygltf::BufferView& bufferView = data.bufferViews[i];
			if (bufferView.target == 0)
			{
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
			if (attrib.first.compare("JOINTS_0") == 0) vertexPointer = 3;
			if (attrib.first.compare("WEIGHTS_0") == 0) vertexPointer = 4;

			if (vertexPointer == 3)
			{
				glVertexAttribIPointer(vertexPointer,
					size,
					accessor.componentType,
					byteStride,
					(char*)0 + accessor.byteOffset);
				glEnableVertexAttribArray(vertexPointer);
			}
			else if (vertexPointer > -1)
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

		//-------------------------Step 5: Load animation and joint data-------------------------------------------------

		//Load joints
		//First loop loads id, name and inverseBindTransform
		auto InverseBindMatricesData = GLTFData(data, data.accessors[skin.inverseBindMatrices]);
		for (int i = 0; i < skin.joints.size(); i++)
		{
			//Access joint node
			tinygltf::Node jointNode = data.nodes[skin.joints[i]];

			//Add joint and store in vector
			if (true) /*jointNode.name.find("IK") == std::string::npos
				&& jointNode.name.find("pole") == std::string::npos
				&& jointNode.name.find("WGT") == std::string::npos
				&& jointNode.name.find("SHP") == std::string::npos
				&& jointNode.name.find("CS") == std::string::npos
				&& jointNode.name.find("POLE") == std::string::npos
				&& jointNode.name.find("FK") == std::string::npos
				&& jointNode.name.find("SPL") == std::string::npos
				&& jointNode.name.find("CTRL") == std::string::npos
				&& jointNode.name.find("CON") == std::string::npos
				&& jointNode.name.find("STR") == std::string::npos
				&& jointNode.name.find("NSTR") == std::string::npos
				&& jointNode.name.find("ASTR") == std::string::npos
				&& jointNode.name.find("NSCA") == std::string::npos
				&& jointNode.name.find("ASCA") == std::string::npos
				&& jointNode.name.find("MM") == std::string::npos
				&& jointNode.name.find("MUSC") == std::string::npos
				&& jointNode.name.find("MCH") == std::string::npos)*/
			{
				newModelData.joints.emplace_back();
				Joint& joint = newModelData.joints[i];
				joint.id = i;
				joint.name = jointNode.name;
				joint.inverseInitialTransform = *InverseBindMatricesData.GetElement<glm::mat4>(i);
			}
		}

		//Second loop loads children
		for (int i = 0; i < newModelData.joints.size(); i++)
		{
			//Access joint node
			tinygltf::Node jointNode = data.nodes[skin.joints[i]];

			for (int& childIndex : jointNode.children)
			{
				//Convert node id to joint id
				bool found = false;
				for (int jointIndex = 0; jointIndex < skin.joints.size() && !found; jointIndex++)
				{
					if (newModelData.joints[jointIndex].name == data.nodes[childIndex].name)
					{
						newModelData.joints[i].children.push_back(&newModelData.joints[jointIndex]);
						found = true;
					}
				}
				assert(found);
			}
		}

		//Set root joints
		for (Joint& j : newModelData.joints)
		{
			if (j.name.find("IK") == std::string::npos && j.name.find("pole") == std::string::npos)
			{
				bool parentFound = false;
				for (Joint potentialParent : newModelData.joints)
				{
					for (Joint* child : potentialParent.children)
					{
						if (child->name == j.name)
						{
							parentFound = true;
						}
					}
				}
				if (!parentFound)
				{
					newModelData.rootJoints.push_back(&j);
				}
			}
		}

		//Load animations
		for (tinygltf::Animation& tinyAnimation : data.animations)
		{
			Animation animation;
			std::unordered_map<int, std::vector<std::pair<float, JointTransform>>> keyFramesPerJoint;	//Maps joint id to keyframes for that node
			size_t nFrames = data.accessors[tinyAnimation.samplers[0].input].count;
			for (int jointIndex = 0; jointIndex < skin.joints.size(); jointIndex++)
			{
				keyFramesPerJoint[jointIndex].resize(nFrames);
			}
			for (tinygltf::AnimationChannel channel : tinyAnimation.channels)
			{
				//Verify that this is a joint (rather than a random joint)
				bool found = false;
				for (int jointIndex = 0; jointIndex < newModelData.joints.size() && !found; jointIndex++)
				{
					if (skin.joints[jointIndex] == channel.target_node)
					{
						found = true;

						//Gain access to data
						tinygltf::AnimationSampler& sampler = tinyAnimation.samplers[channel.sampler];
						tinygltf::Accessor& timeStampAccessor = data.accessors[sampler.input];
						tinygltf::Accessor& transformAccessor = data.accessors[sampler.output];
						GLTFData timeStamps(data, timeStampAccessor);
						GLTFData transforms(data, transformAccessor);

						assert(timeStampAccessor.count == transformAccessor.count);
						assert(timeStampAccessor.count == nFrames);

						//Retrieve data per frame
						for (int i = 0; i < timeStampAccessor.count; i++)
						{
							std::pair<float, JointTransform>& keyFrame = keyFramesPerJoint[jointIndex][i];
							keyFrame.first = *timeStamps.GetElement<float>(i);
							if (channel.target_path == "translation")
							{
								keyFrame.second.position = *transforms.GetElement<glm::vec3>(i);
							}
							else if (channel.target_path == "rotation")
							{
								keyFrame.second.rotation = *transforms.GetElement<glm::quat>(i);
							}
							else if (channel.target_path == "scale")
							{
								//Ignore scale for now
								//REPLACE?
							}
							else
							{
								//REMOVE or REPLACE with exception???
								//It's not actually dangerous to the program if this happens,
								//but may give unexpected results
								std::cout << "WARNING: channel stores unexpected data: " << channel.target_path << std::endl;
							}
						}
					}
				}
			}

			//Loop through frames to take data from individual joints and put it all together
			//size_t nFrames = keyFramesPerJoint[0].size();
			for (int i = 0; i < nFrames; i++)
			{
				animation.frames.emplace_back();
				KeyFrame& currentFrame = animation.frames[i];
				currentFrame.timeStamp = keyFramesPerJoint[0][i].first;
				animation.duration = currentFrame.timeStamp;	//At the end of the loop, this should be set to the timeStamp of the final frame
				for (int jointIndex = 0; jointIndex < newModelData.joints.size(); jointIndex++)
				{
					currentFrame.jointTransforms.push_back(keyFramesPerJoint[jointIndex][i].second);
				}
			}

			assert(newModelData.animations.count(tinyAnimation.name) == 0);	//Animations can't have duplicates
			newModelData.animations[tinyAnimation.name] = animation;
		}

		//-------------------------Step 6: Set up the texture-------------------------------------------------
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
		glGenTextures(1, &newModelData.texture);
		glBindTexture(GL_TEXTURE_2D, newModelData.texture);

		//Set texture settings
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		//Load data into texture
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.width, image.height, 0, format, type, &image.image.at(0));

		GL_ERROR_CHECK();
	}

	return existingModels.at(name);
}

std::vector<glm::mat4> AnimatedModel::GetJointTransforms() const
{
	std::vector<glm::mat4> result;
	for (const Joint& j : modelData.joints)
	{
		result.push_back(j.transform);
	}
	return result;
}

void AnimatedModel::ApplyPoseToJointsRecursively(const std::vector<glm::mat4>& pose, Joint& headJoint, const glm::mat4& parentTransform)
{
	glm::mat4 currentLocalTransform = pose[headJoint.id];
	glm::mat4 currentTransform = parentTransform * currentLocalTransform;
	for (Joint* child : headJoint.children)
	{
		ApplyPoseToJointsRecursively(pose, *child, currentTransform);
	}
	currentTransform *= headJoint.inverseInitialTransform;
	headJoint.transform = currentTransform;
}
