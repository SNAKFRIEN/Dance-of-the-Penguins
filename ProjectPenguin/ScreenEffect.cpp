#include "ScreenEffect.h"

ScreenEffect::ScreenEffect()
	:
	noEffect("PassToFragBackground.vert", "PassToScreen.frag"),
	flashEffect("PassToFragBackground.vert", "FlashEffect.frag")
{
}

void ScreenEffect::Update(float dt)
{
	switch (currentEffectType)
	{
	case EffectType::Flash:
		flashCurrentTime -= dt;
		if (flashCurrentTime <= 0)
		{
			SetNoneEffect();
		}
		break;
	}
}

void ScreenEffect::UseEffect()
{
	switch (currentEffectType)
	{
	case EffectType::None:
		noEffect.Use();
		break;
	case EffectType::Flash:
		flashEffect.Use();
		flashEffect.SetUniformFloat("brightness", flashCurrentTime / flashDuration);
		break;
	}
}

ScreenEffect::EffectType ScreenEffect::GetCurrentEffectType() const
{
	return currentEffectType;
}

void ScreenEffect::SetNoneEffect()
{
	currentEffectType = EffectType::None;
}

void ScreenEffect::SetFlashEffect(float duration)
{
	currentEffectType = EffectType::Flash;
	flashDuration = duration;
	flashCurrentTime = duration;
}
