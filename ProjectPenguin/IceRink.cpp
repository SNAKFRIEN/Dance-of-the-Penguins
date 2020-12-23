#include "IceRink.h"

#include "glm/gtc/matrix_transform.hpp"

#include "Collectible.h"

IceRink::IceRink(bool initModels)
{
	if (initModels)
	{
		InitModels();
	}

	lightSources.emplace_back(-14.29f, 2.64f, -16.96f);	//Left lantern
	lightSources.emplace_back(-4.28f, 2.64f, -17.9f);	//Middle left lantern
	lightSources.emplace_back(4.54, 2.61, -20.86);		//Middle right lantern
	lightSources.emplace_back(12.8f, 2.64f, -15.97f);	//Right lantern
	lightSources.emplace_back(-28.13, 3.77, -12.23);	//Left house0
	lightSources.emplace_back(-31.32, 3.77, -10.29);	//Left house1
	lightSources.emplace_back(9.65, 2.2, -15.83);		//Right house (bottom)
	lightSources.emplace_back(6.77, 2.84, -16.7);		//Right house (top)
	lightSources.emplace_back(-0.76, 3.42, -18.30);		//Middle house

	//REPLACE: Would be better if these could all use the same shader
	ground->GetShader().Use();
	ground->GetShader().SetUniformInt("nSimpleLights", (int)lightSources.size());
	ground->GetShader().SetUniformVec3Array("simpleLights", lightSources);
	market->GetShader().Use();
	market->GetShader().SetUniformInt("nSimpleLights", (int)lightSources.size());
	market->GetShader().SetUniformVec3Array("simpleLights", lightSources);
	lamps->GetShader().Use();
	lamps->GetShader().SetUniformInt("nSimpleLights", (int)lightSources.size());
	lamps->GetShader().SetUniformVec3Array("simpleLights", lightSources);
	trees->GetShader().Use();
	trees->GetShader().SetUniformInt("nSimpleLights", (int)lightSources.size());
	trees->GetShader().SetUniformVec3Array("simpleLights", lightSources);
	restaurant->GetShader().Use();
	restaurant->GetShader().SetUniformInt("nSimpleLights", (int)lightSources.size());
	restaurant->GetShader().SetUniformVec3Array("simpleLights", lightSources);
	mountains->GetShader().Use();
	mountains->GetShader().SetUniformInt("nSimpleLights", (int)lightSources.size());
	mountains->GetShader().SetUniformVec3Array("simpleLights", lightSources);
	backgroundHouses->GetShader().Use();
	backgroundHouses->GetShader().SetUniformInt("nSimpleLights", (int)lightSources.size());
	backgroundHouses->GetShader().SetUniformVec3Array("simpleLights", lightSources);
	house->GetShader().Use();
	house->GetShader().SetUniformInt("nSimpleLights", (int)lightSources.size());
	house->GetShader().SetUniformVec3Array("simpleLights", lightSources);

	transform = glm::mat4(1.0f);
	Reset();
}

void IceRink::DrawStatic(Camera& camera)
{
	ground->AddToRenderQueue(camera);
	market->AddToRenderQueue(camera);
	lamps->AddToRenderQueue(camera);
	trees->AddToRenderQueue(camera);
	restaurant->AddToRenderQueue(camera);
	mountains->AddToRenderQueue(camera);
	backgroundHouses->AddToRenderQueue(camera);
	house->AddToRenderQueue(camera);
	blackBox->AddToRenderQueue(camera);
}

void IceRink::DrawNonStatic(Camera& camera, const std::vector<glm::vec3>& collectiblePositions)
{
	//Bind uniforms for ice shader 
	iceModel->GetShader().Use();
	iceModel->GetShader().SetUniformInt("nCollectibles", (int)collectiblePositions.size());
	if (!collectiblePositions.empty())
	{
		iceModel->GetShader().SetUniformVec3Array("collectibles", collectiblePositions);
	}
	//Draw
	iceModel->AddToRenderQueue(camera);
	iceHole->AddToRenderQueue(camera);
}

void IceRink::Reset()
{
	//Reset ice location
	iceTransform = glm::translate(glm::mat4(1.0f), defaultIcePos);	//Make sure that hole in ice is off screen
}

void IceRink::Update(float elapsedTime)
{
	//REMOVE this function if unused
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
	//Ice
	iceModel = std::make_unique<Model>("Ice.gltf", iceTransform, "SmoothShader.vert", "IceShader.frag");
	iceHole = std::make_unique<Model>("IceHole.gltf", iceTransform);
	//Surroundings
	ground = std::make_unique<Model>("Ground.gltf", transform, "SmoothShader.vert", "Surroundings.frag");
	market = std::make_unique<Model>("Market.gltf", transform, "SmoothShader.vert", "Surroundings.frag");
	lamps = std::make_unique<Model>("Lamps.gltf", transform, "SmoothShader.vert", "Surroundings.frag");
	trees = std::make_unique<Model>("Trees.gltf", transform, "SmoothShader.vert", "Surroundings.frag");
	restaurant = std::make_unique<Model>("Restaurant.gltf", transform, "SmoothShader.vert", "Surroundings.frag");
	mountains = std::make_unique<Model>("Mountains.gltf", transform, "SmoothShader.vert", "Surroundings.frag");
	backgroundHouses = std::make_unique<Model>("BackgroundHouses.gltf", transform, "SmoothShader.vert", "Surroundings.frag");
	house = std::make_unique<Model>("House.gltf", transform, "SmoothShader.vert", "Surroundings.frag");
	blackBox = std::make_unique<Model>("BlackBox.gltf", transform, "SmoothShader.vert", "Background.frag");
}
