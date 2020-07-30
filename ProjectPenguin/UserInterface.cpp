#include "UserInterface.h"

MenuCanvas::MenuCanvas(const Window& window, float aspectRatio)
	:
	window(window),
	aspectRatio(aspectRatio),
	width(2.0f),
	height(2.0f)
{
}

void MenuCanvas::AddButton(glm::vec2 topLeft, glm::vec2 bottomRight, std::string name)
{
	assert(buttons.count(name) == 0);
	float buttonLeft = topLeft.x * width * 0.5f;
	float buttonRight = bottomRight.x * width * 0.5f;
	float buttonTop = topLeft.y * height * 0.5f;
	float buttonBottom = bottomRight.y * height * 0.5f;
	buttons.emplace(name, std::move(UIButton(buttonLeft, buttonTop, buttonRight, buttonBottom, topLeft, bottomRight)));
}

UIButton& MenuCanvas::GetButton(std::string name)
{
	assert(buttons.count(name) != 0);
	return buttons.at(name);
}

void MenuCanvas::Update()
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

	//Update button sizes
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
}

void MenuCanvas::Draw()
{
	//Loop through all UI elements and draw
	for (std::pair<const std::string, UIButton>& button : buttons)
	{
		button.second.Draw();
	}
}