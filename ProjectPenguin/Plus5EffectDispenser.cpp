#include "Plus5EffectDispenser.h"

Plus5EffectDispenser::Plus5EffectDispenser()
{
	Plus5Effect::PreLoad();
}

void Plus5EffectDispenser::Dispense(glm::vec3 pos)
{
	plus5Effects.emplace_back(pos);
}

void Plus5EffectDispenser::Update(float deltaTime)
{
	//Update effects
	for (Plus5Effect& plus5 : plus5Effects)
	{
		plus5.Update(deltaTime);
	}
	//Remove redundant effects
	{
		const auto newEnd = std::remove_if(plus5Effects.begin(), plus5Effects.end(),
			[](Plus5Effect& plus5)
			{
				return plus5.IsFinished();
			});
		plus5Effects.erase(newEnd, plus5Effects.end());
	}
}

void Plus5EffectDispenser::Draw(const Camera& camera)
{
	Plus5Effect::BindGraphics();
	for (Plus5Effect& plus5 : plus5Effects)
	{
		plus5.Draw(camera);
	}
}

void Plus5EffectDispenser::Clear()
{
	plus5Effects.clear();
}
