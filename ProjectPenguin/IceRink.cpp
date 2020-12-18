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
		if (!flowerPositions.empty())
		{
			iceModel->GetShader().SetUniformVec3Array("flowers", flowerPositions);
		}
		//Draw
		iceModel->AddToRenderQueue(camera);
		iceHole->AddToRenderQueue(camera);
	}

	//block1IsPenguin ? penguinBlock1->AddToRenderQueue(camera) : audienceBlock1->AddToRenderQueue(camera);
	//block2IsPenguin ? penguinBlock2->AddToRenderQueue(camera) : audienceBlock2->AddToRenderQueue(camera);
	//block3IsPenguin ? penguinBlock3->AddToRenderQueue(camera) : audienceBlock3->AddToRenderQueue(camera);
	//block4IsPenguin ? penguinBlock4->AddToRenderQueue(camera) : audienceBlock4->AddToRenderQueue(camera);
}

void IceRink::Reset()
{
	//Reset ice location
	iceTransform = glm::translate(glm::mat4(1.0f), defaultIcePos);	//Make sure that hole in ice is off screen
}

void IceRink::Update(float elapsedTime)
{
	//block1Transform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, sin(elapsedTime * block1WiggleSpeedMultiplier) * audienceWiggleStrength, 0.0f));
	//block2Transform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, sin(elapsedTime * block2WiggleSpeedMultiplier) * audienceWiggleStrength, 0.0f));
	//block3Transform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, sin(elapsedTime * block3WiggleSpeedMultiplier) * audienceWiggleStrength, 0.0f));
	//block4Transform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, sin(elapsedTime * block4WiggleSpeedMultiplier) * audienceWiggleStrength, 0.0f));
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

void IceRink::SetAudienceBlock1(bool isPenguin)
{
	block1IsPenguin = isPenguin;
}

void IceRink::SetAudienceBlock2(bool isPenguin)
{
	block2IsPenguin = isPenguin;
}

void IceRink::SetAudienceBlock3(bool isPenguin)
{
	block3IsPenguin = isPenguin;
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
	//Audience
	//audienceBlock1 = std::make_unique<Model>("AudienceBlock1.gltf", block1Transform, "SmoothShader.vert", "AudienceCelShader.frag");
	//audienceBlock2 = std::make_unique<Model>("AudienceBlock2.gltf", block2Transform, "SmoothShader.vert", "AudienceCelShader.frag");
	//audienceBlock3 = std::make_unique<Model>("AudienceBlock3.gltf", block3Transform, "SmoothShader.vert", "AudienceCelShader.frag");
	//audienceBlock4 = std::make_unique<Model>("AudienceBlock4.gltf", block4Transform, "SmoothShader.vert", "AudienceCelShader.frag");
	//penguinBlock1 = std::make_unique<Model>("GoopBlock1.gltf", block1Transform, "SmoothShader.vert", "AudienceCelShader.frag");
	//penguinBlock2 = std::make_unique<Model>("GoopBlock2.gltf", block2Transform, "SmoothShader.vert", "AudienceCelShader.frag");
	//penguinBlock3 = std::make_unique<Model>("GoopBlock3.gltf", block3Transform, "SmoothShader.vert", "AudienceCelShader.frag");
	//penguinBlock4 = std::make_unique<Model>("GoopBlock4.gltf", block4Transform, "SmoothShader.vert", "AudienceCelShader.frag");
}
