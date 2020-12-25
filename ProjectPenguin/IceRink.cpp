#include "IceRink.h"

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/rotate_vector.hpp"

#include "Collectible.h"

IceRink::IceRink(bool initModels)
	:
	ferrisWheelRotationAndTranslationMat(glm::translate(glm::mat4(1.0f), glm::vec3(30.1384f, 8.02308f, -8.90442f))
		* glm::rotate(glm::mat4(1.0f), -0.527923701f, glm::vec3(0.0f, 1.0f, 0.0f))),
	carouselTranslation(glm::translate(glm::mat4(1.0f), glm::vec3(-30.15f, 0.21f, 4.49f)))
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
	lightSources.emplace_back(6.77, 1.93, -16.7);		//Right house (top)
	lightSources.emplace_back(-0.76, 3.42, -18.30);		//Middle house
	lightSources.emplace_back(30.13f, 8.02f, -8.90f);	//Ferris wheel
	lightSources.emplace_back(-30.15f, 1.21f, 4.49f);	//Carousel

	//REPLACE: Would be better if these could all use the same shader (EXCEPT THE BACKGROUND!!!)
	for (Model& m : staticSurroundings)
	{
		m.GetShader().Use();
		m.GetShader().SetUniformInt("nSimpleLights", (int)lightSources.size());
		m.GetShader().SetUniformVec3Array("simpleLights", lightSources);
	}
	carousel->GetShader().Use();
	carousel->GetShader().SetUniformInt("nSimpleLights", (int)lightSources.size());
	carousel->GetShader().SetUniformVec3Array("simpleLights", lightSources);

	transform = glm::mat4(1.0f);

	//Setup initial ferris wheel transform
	ferrisWheelTransform = ferrisWheelRotationAndTranslationMat;
	float ferrisWheelCartRotationOffset = 0.78539816339f;	//Rotation offset per cart
	for (int i = 0; i < ferrisWheelCartTransforms.size(); i++)
	{
		glm::mat4& matrix = ferrisWheelCartTransforms[i];
		glm::vec3 cartPos = glm::rotateZ(glm::vec3(0.0f, 5.55641f, 0.0f), ferrisWheelRotation + ferrisWheelCartRotationOffset * i);
		matrix = ferrisWheelRotationAndTranslationMat * glm::translate(glm::mat4(1.0f), cartPos);
	}
	Reset();
}

void IceRink::DrawStatic(Camera& camera)
{
	for (Model& m : staticSurroundings)
	{
		m.AddToRenderQueue(camera);
	}
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

	//Calculate light source locations for ferris wheel
	std::vector<glm::vec3> ferrisWheelLights;
	ferrisWheelLights.resize(ferrisWheelCartTransforms.size());
	for (int i = 0; i < ferrisWheelCartTransforms.size(); i++)
	{
		ferrisWheelLights[i] = glm::vec3(ferrisWheelCartTransforms[i] * glm::vec4(0.0f, -2.0f, 1.0f, 1.0f));
	}

	//Draw ferris wheel
	ferrisWheel->GetShader().Use();
	ferrisWheel->GetShader().SetUniformInt("nSimpleLights", (int)ferrisWheelLights.size());
	ferrisWheel->GetShader().SetUniformVec3Array("simpleLights", ferrisWheelLights);
	ferrisWheel->AddToRenderQueue(camera);
	
	for (Model& m : ferrisWheelCarts)
	{
		m.GetShader().Use();
		m.GetShader().SetUniformInt("nSimpleLights", (int)ferrisWheelLights.size());
		m.GetShader().SetUniformVec3Array("simpleLights", ferrisWheelLights);
		m.AddToRenderQueue(camera);
	}

	//Draw carousel
	carousel->AddToRenderQueue(camera);
}

void IceRink::Reset()
{
	//Reset ice location
	iceTransform = glm::translate(glm::mat4(1.0f), defaultIcePos);	//Make sure that hole in ice is off screen
}

void IceRink::Update(float deltaTime)
{
	ferrisWheelRotation += deltaTime * 0.3f;
	ferrisWheelTransform = ferrisWheelRotationAndTranslationMat * glm::rotate(glm::mat4(1.0f), ferrisWheelRotation, glm::vec3(0.0f, 0.0f, 1.0f));
	float ferrisWheelCartRotationOffset = 0.78539816339f;	//Rotation offset per cart
	for (int i = 0; i < ferrisWheelCartTransforms.size(); i++)
	{
		glm::mat4& matrix = ferrisWheelCartTransforms[i];
		glm::vec3 cartPos = glm::rotateZ(glm::vec3(0.0f, 5.55641f, 0.0f), ferrisWheelRotation + ferrisWheelCartRotationOffset * i);
		matrix = ferrisWheelRotationAndTranslationMat * glm::translate(glm::mat4(1.0f), cartPos);
	}

	carouselRotation += deltaTime * 0.4f;
	carouselTransform = carouselTranslation * glm::rotate(glm::mat4(1.0f), ferrisWheelRotation, glm::vec3(0.0f, -1.0f, 0.0f));
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
	staticSurroundings.emplace_back("Ground.gltf", transform, "SmoothShader.vert", "Surroundings.frag");
	staticSurroundings.emplace_back("Market.gltf", transform, "SmoothShader.vert", "Surroundings.frag");
	staticSurroundings.emplace_back("Lamps.gltf", transform, "SmoothShader.vert", "Surroundings.frag");
	staticSurroundings.emplace_back("Trees.gltf", transform, "SmoothShader.vert", "Surroundings.frag");
	staticSurroundings.emplace_back("Restaurant.gltf", transform, "SmoothShader.vert", "Surroundings.frag");
	staticSurroundings.emplace_back("Mountains.gltf", transform, "SmoothShader.vert", "Surroundings.frag");
	staticSurroundings.emplace_back("BackgroundHouses.gltf", transform, "SmoothShader.vert", "Surroundings.frag");
	staticSurroundings.emplace_back("House.gltf", transform, "SmoothShader.vert", "Surroundings.frag");
	staticSurroundings.emplace_back("FerrisWheelBase.gltf", transform, "SmoothShader.vert", "Surroundings.frag");
	staticSurroundings.emplace_back("CarouselBase.gltf", transform, "SmoothShader.vert", "Surroundings.frag");
	staticSurroundings.emplace_back("BlackBox.gltf", transform, "SmoothShader.vert", "Background.frag");

	//Animations
	ferrisWheel = std::make_unique<Model>("FerrisWheel.gltf", ferrisWheelTransform, "SmoothShader.vert", "Surroundings.frag");
	ferrisWheelCartTransforms.resize(8);
	ferrisWheelCarts.emplace_back("FerrisWheelCart1.gltf", ferrisWheelCartTransforms[0], "SmoothShader.vert", "Surroundings.frag");
	ferrisWheelCarts.emplace_back("FerrisWheelCart2.gltf", ferrisWheelCartTransforms[1], "SmoothShader.vert", "Surroundings.frag");
	ferrisWheelCarts.emplace_back("FerrisWheelCart3.gltf", ferrisWheelCartTransforms[2], "SmoothShader.vert", "Surroundings.frag");
	ferrisWheelCarts.emplace_back("FerrisWheelCart4.gltf", ferrisWheelCartTransforms[3], "SmoothShader.vert", "Surroundings.frag");
	ferrisWheelCarts.emplace_back("FerrisWheelCart1.gltf", ferrisWheelCartTransforms[4], "SmoothShader.vert", "Surroundings.frag");
	ferrisWheelCarts.emplace_back("FerrisWheelCart2.gltf", ferrisWheelCartTransforms[5], "SmoothShader.vert", "Surroundings.frag");
	ferrisWheelCarts.emplace_back("FerrisWheelCart3.gltf", ferrisWheelCartTransforms[6], "SmoothShader.vert", "Surroundings.frag");
	ferrisWheelCarts.emplace_back("FerrisWheelCart4.gltf", ferrisWheelCartTransforms[7], "SmoothShader.vert", "Surroundings.frag");

	carousel = std::make_unique<Model>("CarouselHorses.gltf", carouselTransform, "SmoothShader.vert", "Surroundings.frag");
}
