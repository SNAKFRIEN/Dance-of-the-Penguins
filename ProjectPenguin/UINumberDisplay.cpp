#include "UINumberDisplay.h"

#include <glad/glad.h>
#include "stb_image.h"

UINumberDisplay::UINumberDisplay(glm::vec2 pos, glm::vec2 letterScale, Anchor anchor, glm::vec2 relativePos, glm::vec2 relativeLetterScale, std::string textureName)
	:
	shader("NumberShader.vert", "NumberShader.frag"),
	relativePos(relativePos),
	relativeLetterScale(relativeLetterScale),
	pos(pos),
	letterScale(letterScale),
	anchor(anchor)
{
	float vertices[] = {
		0.5f, 0.5f,		0.1f, 1.0f,		//Top right
		0.5f, -0.5f,	0.1f, 0.0f,		//Bottom right
		-0.5f, -0.5f,	0.0f, 0.0f,		//Bottom left
		-0.5f, 0.5f,	0.0f, 1.0f		//Top left 
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

UINumberDisplay::~UINumberDisplay()
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

UINumberDisplay::UINumberDisplay(UINumberDisplay&& rhs) noexcept
	:
	vao(rhs.vao),
	vbo(rhs.vbo),
	ebo(rhs.ebo),
	shader(std::move(rhs.shader)),
	texture(rhs.texture),
	relativePos(rhs.relativePos),
	relativeLetterScale(rhs.relativeLetterScale),
	pos(rhs.pos),
	letterScale(rhs.letterScale),
	anchor(rhs.anchor)
{
	rhs.vao = 0;
	rhs.texture = 0;
}

void UINumberDisplay::UpdateSize(glm::vec2 newPos, glm::vec2 newScale)
{
	pos = newPos;
	letterScale = newScale;
}

void UINumberDisplay::SetNumber(unsigned int value)
{
	displayValue.clear();

	if (value == 0)
	{
		displayValue.push_back(0);
	}
	else
	{
		while (value > 0)
		{
			displayValue.push_back(value % 10);
			value /= 10;
		}
		std::reverse(displayValue.begin(), displayValue.end());
	}
}

void UINumberDisplay::Draw()
{
	//Bind graphics stuff
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	shader.Use();

	glBindVertexArray(vao);

	//Calculate position.x of leftmost letter
	float left;
	switch (anchor)
	{
	case Anchor::Left:
		left = pos.x;
		break;
	case Anchor::Center:
		left = pos.x - letterScale.x * (float)displayValue.size() / 2.0f;
		break;
	case Anchor::Right:
		left = pos.x - (float)displayValue.size() * letterScale.x;
		break;
	}
	left += letterScale.x * 0.5f;

	//Render letters one by one
	for (int i = 0; i < displayValue.size(); i++)
	{
		glm::vec2 letterPos = pos;
		letterPos.x = left + letterScale.x * (float)i;
		shader.SetUniformVec2("pos", letterPos);
		shader.SetUniformVec2("scale", letterScale);
		shader.SetUniformInt("value", displayValue[i]);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}

	glBindVertexArray(0);
}

glm::vec2 UINumberDisplay::GetRelativePos() const
{
	return relativePos;
}

glm::vec2 UINumberDisplay::GetRelativeScale() const
{
	return relativeLetterScale;
}
