#include "UserInterface.h"

UICanvas::UICanvas(const Window& window, float aspectRatio)
	:
	window(window),
	aspectRatio(aspectRatio),
	width(2.0f),
	height(2.0f)
{
}

void UICanvas::AddButton(glm::vec2 topLeft, glm::vec2 bottomRight, std::string name, std::string textureName)
{
	assert(buttons.count(name) == 0);
	float buttonLeft = topLeft.x * width * 0.5f;
	float buttonRight = bottomRight.x * width * 0.5f;
	float buttonTop = topLeft.y * height * 0.5f;
	float buttonBottom = bottomRight.y * height * 0.5f;
	buttons.emplace(name, std::move(UIButton(buttonLeft, buttonTop, buttonRight, buttonBottom, topLeft, bottomRight, textureName)));
}

void UICanvas::AddNumberDisplay(glm::vec2 pos, glm::vec2 scale, Anchor anchor, std::string name)
{
	assert(numberDisplays.count(name) == 0);
	glm::vec2 relativePos = pos;
	glm::vec2 relativeScale = scale;
	pos.x *= width * 0.5f;
	pos.y *= height * 0.5f;
	scale.x *= width * 0.5f;
	scale.y *= height * 0.5f;
	numberDisplays.emplace(name, std::move(UINumberDisplay(pos, scale, anchor, relativePos, relativeScale)));
}

UIButton& UICanvas::GetButton(std::string name)
{
	assert(buttons.count(name) != 0);
	return buttons.at(name);
}

UINumberDisplay& UICanvas::GetNumberDisplay(std::string name)
{
	assert(numberDisplays.count(name) != 0);
	return numberDisplays.at(name);
}

void UICanvas::Update()
{
	//Update MenuCanvas size
	width = 2.0f;
	height = 2.0f;

	float windowWidth = window.GetDimensions().x;
	float windowHeight = window.GetDimensions().y;
	float windowAspectRatio = windowWidth / windowHeight;

	if (aspectRatio < windowAspectRatio) //Menu taller than window
	{
		width = aspectRatio * (windowHeight / windowWidth);
		width *= 2.0f;
	}
	else //Menu wider than window
	{
		height = aspectRatio * windowAspectRatio;
		height *= 2.0f;
	}

	//Update UI element sizes
	for (std::pair<const std::string, UIButton>& element : buttons)
	{
		UIButton& button = element.second;
		auto topLeft = button.GetRelativeTopLeft();
		auto bottomRight = button.GetRelativeBottomRight();
		float buttonLeft = topLeft.x * width * 0.5f;
		float buttonRight = bottomRight.x * width * 0.5f;
		float buttonTop = topLeft.y * height * 0.5f;
		float buttonBottom = bottomRight.y * height * 0.5f;
		button.UpdateSize(buttonLeft, buttonTop, buttonRight, buttonBottom);
	}
	for (std::pair<const std::string, UINumberDisplay>& element : numberDisplays)
	{
		UINumberDisplay& numberDisplay = element.second;
		auto pos = numberDisplay.GetRelativePos();
		auto scale = numberDisplay.GetRelativeScale();
		pos.x *= width * 0.5f;
		pos.y *= height * 0.5f;
		scale.x *= width * 0.5f;
		scale.y *= height * 0.5f;
		numberDisplay.UpdateSize(pos, scale);
	}
}

void UICanvas::Draw()
{
	//Turn on blending
	glEnable(GL_BLEND);
	//Loop through all UI elements and draw
	for (std::pair<const std::string, UIButton>& button : buttons)
	{
		button.second.Draw();
	}
	for (std::pair<const std::string, UINumberDisplay>& numberDisplay : numberDisplays)
	{
		numberDisplay.second.Draw();
	}
	//Turn off blending
	glDisable(GL_BLEND);
}