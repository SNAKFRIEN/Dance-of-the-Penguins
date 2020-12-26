#include "UIButton.h"

#include "Input.h"

#include <glad/glad.h>
#include "stb_image.h"

UIButton::UIButton(float left, float top, float right, float bottom, glm::vec2 relativeTopLeft, glm::vec2 relativeBottomRight, std::string textureName)
	:
	shader("UIShader.vert", "UIShader.frag"),
	left(left),
	top(top),
	right(right),
	bottom(bottom),
	relativeTopLeft(relativeTopLeft),
	relativeBottomRight(relativeBottomRight)
{
	float vertices[] = {
		right, top,		1.0f, 1.0f,		//Top right
		right, bottom,	1.0f, 0.0f,		//Bottom right
		left, bottom,	0.0f, 0.0f,		//Bottom left
		left, top,		0.0f, 1.0f		//Top left 
	};
	unsigned int indices[] = {
		3, 1, 0,   // first triangle
		3, 2, 1    // second triangle
	};

	//Generate VAO
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	//Generate VBO
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

	//Generate EBO
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//Set up attributes
	//REPLACE with that function to find attribute index
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//Generate texture
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	//Set texture settings
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//Load image data into texture
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);
	std::string texturePath = "UI/";
	texturePath.append(textureName);
	unsigned char* data = stbi_load(texturePath.c_str(), &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::string errorMessage = "The UI texture ";
		errorMessage.append(texturePath);
		errorMessage.append(" could not be loaded");
		throw std::exception(errorMessage.c_str());
	}
	stbi_set_flip_vertically_on_load(false);
	stbi_image_free(data);

	//Set shader uniforms
	shader.Use();
	shader.SetUniformInt("texture0", 0);
}

UIButton::~UIButton()
{
	if (vao > 0)
	{
		glDeleteVertexArrays(1, &vao);
		glDeleteBuffers(1, &vbo);
		glDeleteBuffers(1, &ebo);
	}
	if (texture > 0)
	{
		glDeleteTextures(1, &texture);
	}
}

UIButton::UIButton(UIButton&& rhs) noexcept
	:
	vao(rhs.vao),
	vbo(rhs.vbo),
	ebo(rhs.ebo),
	shader(std::move(rhs.shader)),
	texture(rhs.texture),
	left(rhs.left),
	right(rhs.right),
	top(rhs.top),
	bottom(rhs.bottom),
	relativeTopLeft(rhs.relativeTopLeft),
	relativeBottomRight(rhs.relativeBottomRight),
	color(rhs.color)
{
	rhs.vao = 0;
	rhs.texture = 0;
}

void UIButton::UpdateSize(float newLeft, float newTop, float newRight, float newBottom)
{
	left = newLeft;
	top = newTop;
	right = newRight;
	bottom = newBottom;

	float vertices[] = {
		right, top,		1.0f, 1.0f,		//Top right
		right, bottom,	1.0f, 0.0f,		//Bottom right
		left, bottom,	0.0f, 0.0f,		//Bottom left
		left, top,		0.0f, 1.0f		//Top left 
	};

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
}

bool UIButton::UpdateAndCheckClick(const Input& input)
{
	float mouseX = input.GetMouseUV().x;
	float mouseY = input.GetMouseUV().y;
	if (mouseX < right
		&& mouseX > left
		&& mouseY < top
		&& mouseY > bottom)
	{
		color = onColor;
		if (input.LMBShortPressed())
		{
			return true;
		}
	}
	else
	{
		color = offColor;
	}
	return false;
}

void UIButton::Draw()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	shader.Use();
	shader.SetUniformVec3("color", color);

	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void UIButton::SetOnColor(glm::vec3 newColor)
{
	onColor = newColor;
	color = newColor;
}

void UIButton::SetOffColor(glm::vec3 newColor)
{
	offColor = newColor;
	color = newColor;
}

glm::vec2 UIButton::GetRelativeTopLeft() const
{
	return relativeTopLeft;
}

glm::vec2 UIButton::GetRelativeBottomRight() const
{
	return relativeBottomRight;
}

float UIButton::GetLeft() const
{
    return left;
}

float UIButton::GetRight() const
{
	return right;
}

float UIButton::GetTop() const
{
	return top;
}

float UIButton::GetBottom() const
{
	return bottom;
}
