#pragma once

#include <unordered_map>
#include <vector>

#include "JointTransform.h"

struct KeyFrame
{
	std::vector<JointTransform> jointTransforms;	//Transformation is in relation to the parent joint!
	float timeStamp;								//Timestamp of this keyframe

	//Returns value between 0 and 1 to indicate the progression from frameA to frameB at the currentTime
	static float CalculateProgression(const KeyFrame& frameA, const KeyFrame& frameB, float currentTime)
	{
		float totalTime = frameB.timeStamp - frameA.timeStamp;
		float deltaTime = currentTime - frameA.timeStamp;	//Time passed since framA
		return deltaTime / totalTime;
	}

	static std::vector<glm::mat4> Interpolate(const KeyFrame& frameA, const KeyFrame& frameB, float alpha)
	{
		std::vector<glm::mat4> result;
		for (int i = 0; i < frameA.jointTransforms.size(); i++)
		{
			JointTransform prevTransform = frameA.jointTransforms[i];
			JointTransform nextTransform = frameB.jointTransforms[i];
			JointTransform currentTransform = JointTransform::Interpolate(prevTransform, nextTransform, alpha);
			result.push_back(currentTransform.GetLocalTransform());
		}
		return result;
	}
};