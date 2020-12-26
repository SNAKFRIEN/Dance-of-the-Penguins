#include "PenguinWarning.h"

#include <glad/glad.h>
#include "stb_image.h"
#include "glm/gtc/matrix_transform.hpp"

#include "Camera.h"

bool PenguinWarning::preloaded = false;
unsigned int PenguinWarning::vao = 0;
unsigned int PenguinWarning::vbo;
unsigned int PenguinWarning::ebo;
std::unique_ptr<Shader> PenguinWarning::shader;
unsigned int PenguinWarning::texture;

PenguinWarning::PenguinWarning()
{
	assert(preloaded);
}

void PenguinWarning::PreLoad()
{
	//REPLACE: I'm not sure if I should allow the PreLoad() function to be called when this class is already preloaded
	if (!preloaded)
	{
		preloaded = true;

		shader = std::make_unique<Shader>("PenguinWarning.vert", "PenguinWarning.frag");

		float vertices[] = {
			1.0f, 1.0f,		1.0f, 1.0f,		//Top right
			1.0f, -1.0f,	1.0f, 0.0f,		//Bottom right
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
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

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
		shader->Use();
		shader->SetUniformInt("texture0", 0);
	}
}

void PenguinWarning::Update(glm::vec3 penguinPos, const Camera& camera, glm::vec2 windowDimensions)
{
	glm::vec4 v4TargetPos = camera.GetVPMatrix() * glm::vec4(penguinPos.x, penguinPos.y + yOffset, penguinPos.z, 1.0f);
	glm::vec2 targetPos = glm::vec2(v4TargetPos) / v4TargetPos.w;

	float xMinBorderDistance = yMinBorderDistance * (windowDimensions.y / windowDimensions.x);
	glm::vec2 minBorderDistance(xMinBorderDistance, yMinBorderDistance);
	pos = glm::clamp(targetPos, glm::vec2(-1.0f) + minBorderDistance, glm::vec2(1.0f) - minBorderDistance);
}

void PenguinWarning::UpdateWidth(float inWidth)
{
	width = inWidth;
}

void PenguinWarning::BindGraphics()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	glBindVertexArray(vao);
}

void PenguinWarning::Draw()
{
	shader->Use();
	shader->SetUniformVec2("pos", pos);
	shader->SetUniformVec2("scale", glm::vec2(width, height));

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

float PenguinWarning::GetHeight() const
{
	return height;
}
