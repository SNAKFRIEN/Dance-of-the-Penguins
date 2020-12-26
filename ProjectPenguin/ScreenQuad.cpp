#include "ScreenQuad.h"

#include "glad/glad.h"

#include "Window.h"
#include "SaveFile.h"

//REMOVE use the proper error check method
#include <sstream>
#include <iomanip>
#include "GlGetError.h"

ScreenQuad::ScreenQuad(const Window& window, const SaveFile& settings)
	:
	window(window),
	settings(settings)
{
	float vertices[] = {
		1.0f, -1.0f,	1.0f, 0.0f,		//Top right
		1.0f, 1.0f,		1.0f, 1.0f,		//Bottom right
		-1.0f, 1.0f,	0.0f, 1.0f,		//Bottom left
		-1.0f, -1.0f,	0.0f, 0.0f		//Top left 
	};
	unsigned int indices[] = {
		0, 1, 3,   // first triangle
		1, 2, 3    // second triangle
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

	//--------------------------------------------------
	//-------------- Set up msFbo ----------------------
	//--------------------------------------------------

	//Set up msFbo
	glGenFramebuffers(1, &msFbo);
	glBindFramebuffer(GL_FRAMEBUFFER, msFbo);

	//Generate msTexture
	glGenTextures(1, &msTexture);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, msTexture);

	GL_ERROR_CHECK();

	//Set texture settings
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, settings.GetMsaaQuality(), GL_RGB, window.GetWidth(), window.GetHeight(), GL_TRUE);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

	GL_ERROR_CHECK();

	//Bind msTexture as color attachment
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, msTexture, 0);

	//Setup msRbo
	glGenRenderbuffers(1, &msRbo);
	glBindRenderbuffer(GL_RENDERBUFFER, msRbo);

	//Bind msRbo as depth and stencil attachment
	//msaaQuality == nSamples
	glBindRenderbuffer(GL_RENDERBUFFER, msRbo);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, settings.GetMsaaQuality(), GL_DEPTH24_STENCIL8, window.GetWidth(), window.GetHeight());
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, msRbo);

	//--------------------------------------------------
	//-------------- Set up regular fbo ----------------
	//--------------------------------------------------

	//Set up fbo
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	//Generate texture
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	//Set texture settings
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, window.GetWidth(), window.GetHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	GL_ERROR_CHECK();
}

void ScreenQuad::StartFrame()
{
	//Bind msFbo
	glBindFramebuffer(GL_FRAMEBUFFER, msFbo);
	GL_ERROR_CHECK();

	//Clear buffer
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	GL_ERROR_CHECK();

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		//REPLACE with better exception message
		throw std::exception("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
	}
}

void ScreenQuad::EndFrame()
{
	//Bind fbo
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	//REMOVE this check?
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		//REPLACE with better exception message
		throw std::exception("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
	}

	//Copy from msFbo to fbo
	glBindFramebuffer(GL_READ_FRAMEBUFFER, msFbo);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
	glBlitFramebuffer(0, 0, window.GetWidth(), window.GetHeight(), 0, 0, window.GetWidth(), window.GetHeight(), GL_COLOR_BUFFER_BIT, GL_NEAREST);

	//Unbind
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ScreenQuad::Draw()
{
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void ScreenQuad::UpdateDimensions()
{
	//--------------------------------------------------
	//-------------- Update msFbo ----------------------
	//--------------------------------------------------

	glBindFramebuffer(GL_FRAMEBUFFER, msFbo);

	//Update msTexture
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, msTexture);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, settings.GetMsaaQuality(), GL_RGB, window.GetWidth(), window.GetHeight(), GL_TRUE);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, msTexture, 0);

	//Update msRbo
	glBindRenderbuffer(GL_RENDERBUFFER, msRbo);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, settings.GetMsaaQuality(), GL_DEPTH24_STENCIL8, window.GetWidth(), window.GetHeight());
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, msRbo);

	//--------------------------------------------------
	//-------------- Update regular fbo ----------------
	//--------------------------------------------------

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	//Update texture
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, window.GetWidth(), window.GetHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

unsigned int ScreenQuad::GetTexture() const
{
	return texture;
}
