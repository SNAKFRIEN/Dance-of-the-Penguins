#include "IceRink.h"

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/rotate_vector.hpp"

#include "Collectible.h"

IceRink::IceRink(bool initModels)
	:
	ferrisWheelRotationAndTranslationMat(glm::translate(glm::mat4(1.0f), glm::vec3(30.1384f, 8.02308f, -8.90442f))
		* glm::rotate(glm::mat4(1.0f), -0.527923701f, glm::vec3(0.0f, 1.0f, 0.0f))),
	carouselTranslation(glm::translate(glm::mat4(1.0f), glm::vec3(-30.15f, -0.21f, 4.49f)))
{
	if (initModels)
	{
		InitModels();
	}

	lightSources.emplace_back(-14.29f, 2.64f, -16.96f);		//Left lantern
	lightSources.emplace_back(-4.28f, 2.64f, -17.9f);		//Middle left lantern
	lightSources.emplace_back(4.54f, 2.61f, -20.86f);		//Middle right lantern
	lightSources.emplace_back(12.8f, 2.64f, -15.97f);		//Right lantern
	lightSources.emplace_back(-28.13f, 3.77f, -12.23f);		//Left house0

	lightSources.emplace_back(-31.32f, 3.77f, -10.29f);		//Left house1
	lightSources.emplace_back(9.65f, 2.2f, -15.83f);		//Right house (bottom)
	lightSources.emplace_back(6.77f, 1.93f, -16.7f);		//Right house (top)
	lightSources.emplace_back(-0.76f, 3.42f, -18.30f);		//Middle house
	lightSources.emplace_back(30.13f, 8.02f, -8.90f);		//Ferris wheel
	
	lightSources.emplace_back(-30.15f, 2.6f, 4.49f);		//Carousel middle
	lightSources.emplace_back(-27.27f, 2.45f, -4.33f);		//Choir
	lightSources.emplace_back(28.0f, 2.0f, 0.67f);			//Far right lantern
	lightSources.emplace_back(-27.15f, 3.0f, 4.49f);			//Carousel right
	lightSources.emplace_back(-30.15f, 3.0f, 7.0f);			//Carousel front


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

	//Setup initial ferris wheel transform (this used to be necessary, but not really anymore)
	ferrisWheelTransform = ferrisWheelRotationAndTranslationMat;
	float ferrisWheelCartRotationOffset = 0.78539816339f;	//Rotation offset per cart
	for (int i = 0; i < ferrisWheelCartTransforms.size(); i++)
	{
		glm::mat4& matrix = ferrisWheelCartTransforms[i];
		glm::vec3 cartPos = glm::rotateZ(glm::vec3(0.0f, 5.55641f, 0.0f), ferrisWheelRotation + ferrisWheelCartRotationOffset * i);
		matrix = ferrisWheelRotationAndTranslationMat * glm::translate(glm::mat4(1.0f), cartPos);
	}
	
	//Setup initial snow throwing penguin transforms
	for (AnimatedModel& p : snowFightingPenguins)
	{
		p.Update(0.0f);
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

	//Draw snow fighting penguins
	for (int i = 0; i < snowFightingPenguins.size(); i++)
	{
		snowFightingPenguins[i].AddToRenderQueue(camera);
		snowBalls[i].AddToRenderQueue(camera);
	}
}

void IceRink::Reset()
{
	//Reset ice location
	iceTransform = glm::translate(glm::mat4(1.0f), defaultIcePos);	//Make sure that hole in ice is off screen
}

void IceRink::Update(float deltaTime)
{
	snowThrowTimer -= deltaTime;
	snowPickTimer -= deltaTime;
	if (snowThrowTimer <= 0.0f)
	{
		snowBallCurveTimes[activeSnowPenguinIndex] = -0.4f;	//Reset throw
		snowFightingPenguins[activeSnowPenguinIndex].SetAnimation("ThrowingSnowball");
		snowFightingPenguins[activeSnowPenguinIndex].SetLooping(false);

		activeSnowPenguinIndex++;
		activeSnowPenguinIndex %= snowFightingPenguins.size();
		snowThrowTimer += timeBetweenSnowThrows;
	}
	if (snowPickTimer <= 0.0f)
	{
		snowFightingPenguins[activeSnowPenguinIndex].SetAnimation("PickingSnowball");
		snowFightingPenguins[activeSnowPenguinIndex].SetLooping(false);
		snowPickTimer += timeBetweenSnowThrows;
	}

	for (int i = 0; i < snowBalls.size(); i++)
	{
		if (snowBallCurveTimes[i] < 100.0f)
		{
			snowBallCurveTimes[i] += deltaTime;

			//Calculate curve
			float t = snowBallCurveTimes[i] * snowBallSpeed;
			float y = -pow(t - 1.0f, 2.0f) + 1.0f; //Curve goes through (0, 0), up to (1, 1) and back down to (2, 0)

			//Transform curve to current position
			glm::vec3 curvePos = glm::vec3(0.0f, y, -t) * snowBallCurveScale;
			snowBallTransforms[i] = snowFightingPenguinTransforms[i] * glm::translate(glm::mat4(1.0f), curvePos);
		}
	}

	for (AnimatedModel& p : snowFightingPenguins)
	{
		if ((p.GetAnimation() == "ThrowingSnowball" || p.GetAnimation() == "PickingSnowball")
			&& p.IsFinished())
		{
			p.SetAnimation("Idle");
			p.SetLooping(true);
		}
		p.Update(deltaTime);
	}

	UpdateFerrisWheelAndCarousel(deltaTime);
}

void IceRink::UpdateFerrisWheelAndCarousel(float deltaTime)
{
	ferrisWheelRotation += deltaTime * 0.3f;
	ferrisWheelTransform = ferrisWheelRotationAndTranslationMat * glm::rotate(glm::mat4(1.0f), ferrisWheelRotation, glm::vec3(0.0f, 0.0f, -1.0f));
	float ferrisWheelCartRotationOffset = 0.78539816339f;	//Rotation offset per cart
	for (int i = 0; i < ferrisWheelCartTransforms.size(); i++)
	{
		glm::mat4& matrix = ferrisWheelCartTransforms[i];
		glm::vec3 cartPos = glm::rotate(glm::vec3(0.0f, 5.55641f, 0.0f), ferrisWheelRotation + ferrisWheelCartRotationOffset * i, glm::vec3(0.0f, 0.0f, -1.0f));
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
	staticSurroundings.emplace_back("Benches.gltf", transform, "SmoothShader.vert", "Surroundings.frag");
	staticSurroundings.emplace_back("Snowmen.gltf", transform, "SmoothShader.vert", "Surroundings.frag");
	staticSurroundings.emplace_back("ChoirStand.gltf", transform, "SmoothShader.vert", "Surroundings.frag");
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

	carousel = std::make_unique<Model>("CarouselHorses.gltf", carouselTransform, "SmoothShader.vert", "Surroundings.frag");	//Import the puppies

	const int nSnowPenguins = 4;
	snowBallTransforms.resize(nSnowPenguins);
	snowFightingPenguinTransforms.emplace_back(glm::rotate(glm::translate(glm::mat4(1.0f), glm::vec3(-9.48f, 0.01f, -15.96f)), 1.57079f, glm::vec3(0.0f, 1.0f, 0.0f)));
	snowFightingPenguinTransforms.emplace_back(glm::rotate(glm::translate(glm::mat4(1.0f), glm::vec3(-9.17f, 0.01f, -15.02f)), 1.57079f, glm::vec3(0.0f, 1.0f, 0.0f)));
	snowFightingPenguinTransforms.emplace_back(glm::rotate(glm::translate(glm::mat4(1.0f), glm::vec3(-17.6f, 0.01f, -15.02f)), -1.57079f, glm::vec3(0.0f, 1.0f, 0.0f)));
	snowFightingPenguinTransforms.emplace_back(glm::rotate(glm::translate(glm::mat4(1.0f), glm::vec3(-15.43f, 0.01f, -15.96f)), -1.57079f, glm::vec3(0.0f, 1.0f, 0.0f)));
	for (int i = 0; i < nSnowPenguins; i++)
	{
		snowFightingPenguins.emplace_back("Goopie.gltf", snowFightingPenguinTransforms[i], "Idle");
		snowBalls.emplace_back("Snowball.gltf", snowBallTransforms[i]);
		snowBallCurveTimes.emplace_back(100.0f);	//Snowballs start way off screen
	}
}
