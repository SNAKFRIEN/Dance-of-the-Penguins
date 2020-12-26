#include "SmokeEffect.h"

#include <glad/glad.h>
#include "stb_image.h"
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtx/rotate_vector.hpp>

#include "Camera.h"

bool SmokeEffect::preloaded = false;
unsigned int SmokeEffect::vao = 0;
unsigned int SmokeEffect::vbo;
unsigned int SmokeEffect::ebo;
std::unique_ptr<Shader> SmokeEffect::shader;
unsigned int SmokeEffect::texture;

SmokeEffect::SmokeEffect(glm::vec3 inPos)
	:
	pos(inPos)
{
	assert(preloaded);
	translation = glm::translate(glm::mat4(1.0f), pos);
}

void SmokeEffect::PreLoad()
{
	assert(!preloaded);
	preloaded = true;

	shader = std::make_unique<Shader>("SmokeAnimation.vert", "SmokeAnimation.frag");

	float vertices[] = {
		1.0f, 1.0f,		frameWidth, 1.0f,		//Top right
		1.0f, -1.0f,	frameWidth, 0.0f,		//Bottom right
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
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//Load image data into texture
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);
	std::string texturePath = "UI/Clouds.png";
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
	shader->SetUniformFloat("frameWidth", frameWidth);
}

void SmokeEffect::Update(float deltaTime)
{
	currentTime += deltaTime;
	currentFrame = (int)(currentTime / timePerFrame);
	finished = currentFrame >= nFrames;
}

void SmokeEffect::BindGraphics()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	shader->Use();

	glBindVertexArray(vao);
}

void SmokeEffect::Draw(const Camera& camera)
{
	//Make effect face the camera at all times
	glm::mat4 transform = translation * glm::orientation(glm::vec3(0.0f, 1.0f, 0.0f), normalize(camera.GetPos() - pos));
	shader->SetUniformMat4("model", transform);
	shader->SetUniformMat4("mvp", camera.GetVPMatrix() * transform);
	shader->SetUniformInt("currentFrame", currentFrame);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

bool SmokeEffect::IsFinished() const
{
	return finished;
}
