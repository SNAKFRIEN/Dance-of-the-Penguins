#pragma once

#include <string>
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include <glm/glm.hpp>

class Camera;

class Window
{
public:
	//Special member functions
	Window(int width, int height, std::string name = "This is a window");
	~Window();
	Window(const Window&) = delete;
	Window operator=(Window&) = delete;
	Window(const Window&&) = delete;
	Window operator=(Window&&) = delete;

	//GLFW
	void BeginFrame();
	void EndFrame();
	void SetTitle(std::string name);
	void Close();
	bool IsClosing() const;
	bool KeyIsPressed(int key) const;
	bool MouseButtonIsPressed(int button) const;
	glm::vec2 GetCursorPos() const;
	glm::vec2 GetDimensions() const;

	//Custom functionality
	void SetMainCamera(Camera* camera);
private:
	GLFWwindow* window = nullptr;
	
	Camera* mainCamera = nullptr;

	int currentWidth;
	int currentHeight;
};