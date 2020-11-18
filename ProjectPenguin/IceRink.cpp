#include "IceRink.h"

#include "glm/gtc/matrix_transform.hpp"

#include "Collectible.h"

IceRink::IceRink(bool initModels)
{
	if (initModels)
	{
		InitModels();
	}
	transform = glm::mat4(1.0f);
	Reset();
}

void IceRink::DrawStatic(Camera& camera, Input& input)
{
	if (!input.IsPressed(70))
	{
		stadiumModel->AddToRenderQueue(camera);
	}

	seatsRight->AddToRenderQueue(camera);
	seatsLeft->AddToRenderQueue(camera);
	seatsTopRight->AddToRenderQueue(camera);
	seatsTopLeft->AddToRenderQueue(camera);
	seatsBottomRight->AddToRenderQueue(camera);
	seatsBottomLeft->AddToRenderQueue(camera);
}

void IceRink::DrawNonStatic(Camera& camera, Input& input, const std::vector<glm::vec3>& flowerPositions)
{
	if (!input.IsPressed(70))
	{
		//Bind uniforms for ice shader 
		iceModel->GetShader().Use();
		iceModel->GetShader().SetUniformInt("nFlowers", (int)flowerPositions.size());
		iceModel->GetShader().SetUniformVec3Array("flowers", flowerPositions);
		//Draw
		iceModel->AddToRenderQueue(camera);
		iceHole->AddToRenderQueue(camera);
	}
}

void IceRink::Reset()
{
	//Reset ice location
	iceTransform = glm::translate(glm::mat4(1.0f), defaultIcePos);	//Make sure that hole in ice is off screen
}

float IceRink::GetRight() const
{
	return right;
}

float IceRink::GetTop() const
{
	return top;
}

float IceRink::GetCornerRadius() const
{
	return cornerRadius;
}

void IceRink::SetIcePos(glm::vec3 newPos)
{
	iceTransform = glm::translate(glm::mat4(1.0f), newPos);
}

void IceRink::InitModels()
{
	stadiumModel = std::make_unique<Model>("Stadium.gltf", transform, "SmoothShader.vert", "AudienceShader.frag");
	iceModel = std::make_unique<Model>("Ice.gltf", iceTransform, "SmoothShader.vert", "IceShader.frag");
	iceHole = std::make_unique<Model>("IceHole.gltf", iceTransform);
	//Seats (loaded in batches)
	seatsRight = std::make_unique<Model>("SeatsRight.gltf", transform, "SmoothShader.vert", "AudienceShader.frag");
	seatsLeft = std::make_unique<Model>("SeatsLeft.gltf", transform, "SmoothShader.vert", "AudienceShader.frag");
	seatsTopRight = std::make_unique<Model>("SeatsTopRight.gltf", transform, "SmoothShader.vert", "AudienceShader.frag");
	seatsTopLeft = std::make_unique<Model>("SeatsTopLeft.gltf", transform, "SmoothShader.vert", "AudienceShader.frag");
	seatsBottomRight = std::make_unique<Model>("SeatsBottomRight.gltf", transform, "SmoothShader.vert", "AudienceShader.frag");
	seatsBottomLeft = std::make_unique<Model>("SeatsBottomLeft.gltf", transform, "SmoothShader.vert", "AudienceShader.frag");
}
