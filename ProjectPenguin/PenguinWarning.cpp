#include "PenguinWarning.h"

#include <glad/glad.h>
#include "stb_image.h"
#include "glm/gtc/matrix_transform.hpp"

#include "Camera.h"

PenguinWarning::PenguinWarning()
	:
	shader("PenguinWarning.vert", "PenguinWarning.frag")
{
	float vertices[] = {
		1.0f, 1.0f,		0.1f, 1.0f,		//Top right
		1.0f, -1.0f,	0.1f, 0.0f,		//Bottom right
		-1.0f, -1.0f,	0.0f, 0.0f,		//Bottom left
		-1.0f, 1.0f,	0.0f, 1.0f		//Top left 
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
	std::string texturePath = "UI/PenguinWarning.png";
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

void PenguinWarning::Update(glm::vec3 penguinPos, const Camera& camera)
{
	glm::vec4 v4TargetPos = glm::vec4(0, 0, 0, 1) * glm::translate(glm::mat4(1), penguinPos) * camera.GetVPMatrix();
	glm::vec2 targetPos = glm::vec2(v4TargetPos.x, v4TargetPos.y);

	std::cout << "WarningPos: " << targetPos.x << ", " << targetPos.y << std::endl;

	//REPLACE calculation so that minBorderDistance is consistent across aspect ratios
	pos = targetPos;	//glm::clamp(targetPos, glm::vec2(0.0f + minBorderDistance), glm::vec2(1.0f - minBorderDistance));
}

void PenguinWarning::UpdateWidth(float inWidth)
{
	width = inWidth;
}

void PenguinWarning::Draw()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	shader.Use();
	shader.SetUniformVec2("pos", pos);
	shader.SetUniformVec2("scale", glm::vec2(width, height));

	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

float PenguinWarning::GetHeight() const
{
	return height;
}
