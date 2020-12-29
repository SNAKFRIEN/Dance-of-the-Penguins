#include "SmokeMachine.h"

SmokeMachine::SmokeMachine()
{
	SmokeEffect::PreLoad();
}

void SmokeMachine::SpawnSmoke(glm::vec3 pos)
{
	smokeEffects.emplace_back(glm::vec3(pos.x, 1.0f, pos.z));
}

void SmokeMachine::Update(float deltaTime)
{
	//Update smokes
	for (SmokeEffect& smoke : smokeEffects)
	{
		smoke.Update(deltaTime);
	}
	//Remove redundant smokes
	{
		const auto newEnd = std::remove_if(smokeEffects.begin(), smokeEffects.end(),
			[](SmokeEffect& smoke)
			{
				return smoke.IsFinished();
			});
		smokeEffects.erase(newEnd, smokeEffects.end());
	}
}

void SmokeMachine::Draw(const Camera& camera)
{
	SmokeEffect::BindGraphics();
	for (SmokeEffect& smoke : smokeEffects)
	{
		smoke.Draw(camera);
	}
}

void SmokeMachine::Clear()
{
	smokeEffects.clear();
}
