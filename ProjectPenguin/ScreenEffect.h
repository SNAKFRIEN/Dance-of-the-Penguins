#pragma once

#include "Shader.h"

class ScreenEffect
{
public:
	enum class EffectType
	{
		None,
		Flash
	};
public:
	ScreenEffect();

	void Update(float dt);
	void UseEffect();

	EffectType GetCurrentEffectType() const;

	void SetNoneEffect();
	void SetFlashEffect(float duration);
private:
	EffectType currentEffectType = EffectType::None;

	Shader noEffect;
	Shader flashEffect;
	float flashDuration;
	float flashCurrentTime;
};