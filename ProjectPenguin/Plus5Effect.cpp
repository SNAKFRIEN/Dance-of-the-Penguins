#include "Plus5Effect.h"

#include <glad/glad.h>
#include "stb_image.h"
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtx/rotate_vector.hpp>

#include "Camera.h"

bool Plus5Effect::preloaded = false;
unsigned int Plus5Effect::vao = 0;
unsigned int Plus5Effect::vbo;
unsigned int Plus5Effect::ebo;
std::unique_ptr<Shader> Plus5Effect::shader;
unsigned int Plus5Effect::texture;

Plus5Effect::Plus5Effect(glm::vec3 inPos)
	:
	pos(inPos)
{
	assert(preloaded);
}

void Plus5Effect::PreLoad()
{
	assert(!preloaded);
	preloaded = true;

	shader = std::make_unique<Shader>("BillBoard.vert", "PassToScreen.frag");

	float vertices[] = {
		0.2f, 0.2f,		1.0f, 1.0f,		//Top right
		0.2f, -0.2f,	1.0f, 0.0f,		//Bottom right
		-0.2f, -0.2f,	0.0f, 0.0f,		//Bottom left
		-0.2f, 0.2f,	0.0f, 1.0f		//Top left
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
	std::string texturePath = "UI/+5.png";
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

void Plus5Effect::Update(float deltaTime)
{
	currentTime += deltaTime;
	pos.y += speed * deltaTime;
}

void Plus5Effect::BindGraphics()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	shader->Use();

	glBindVertexArray(vao);
}

void Plus5Effect::Draw(const Camera& camera)
{
	//Make effect face the camera at all times
	glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * glm::rotate(glm::mat4(1.0f), glm::radians(-45.0f), glm::vec3(1.0f, 0.0, 0.0f));
	shader->SetUniformMat4("model", transform);
	shader->SetUniformMat4("mvp", camera.GetVPMatrix() * transform);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

bool Plus5Effect::IsFinished() const
{
	return currentTime > maxLifeTime;
}