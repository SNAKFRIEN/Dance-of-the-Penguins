#pragma once

#define TINYGLTF_NO_STB_IMAGE_WRITE
#include "tiny_gltf.h"
#include <glm/glm.hpp>

#include "Shader.h"
#include "Joint.h"
#include "KeyFrame.h"

class Camera;

class AnimatedModel
{
private:
	struct Animation
	{
		float duration;
		std::vector<KeyFrame> frames;
	};
public:
	AnimatedModel(std::string name, const glm::mat4& ownerTransform);
	void Update(float dt);
	void Draw(Camera& camera);
private:
	std::vector<glm::mat4> GetJointTransforms() const;	//Retrieve transform per joint
	void ApplyPoseToJointsRecursively(const std::vector<glm::mat4>& pose, Joint& headJoint, const glm::mat4& parentTransform);
private:
	//Geometry
	unsigned int vao = 0;
	size_t nIndices = 0;

	//Shader
	Shader shader;

	//Texture
	unsigned int texture = 0;	//Only supports models with single textures for now

	//Animation
	std::vector<Joint> joints;
	std::vector<Joint*> rootJoints;
	//int nJoints = 0;

	std::unordered_map<std::string, Animation> animations;	//Map of all the animations in this model
	std::string currentAnimation = "waddle_neutral_slow";	//Name of current animation
	float animationTime = 0.0f;	//Current time in animation

	//Reference to owner transform
	const glm::mat4& ownerTransform;
};