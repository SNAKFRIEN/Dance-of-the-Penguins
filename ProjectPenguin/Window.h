#pragma once

#include <string>
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include <glm/glm.hpp>

class Camera;
class ScreenQuad;

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
	int GetWidth() const;
	int GetHeight() const;
	void ShowMouse() const;
	void HideMouse() const;

	//Custom functionality
	void SetMainCamera(Camera* camera);
	void SetScreenQuad(ScreenQuad* screenQuad);
private:
	void ResizeCallback();
private:
	GLFWwindow* window = nullptr;
	
	//Automatically adjust properties when screen size changes
	Camera* mainCamera = nullptr;
	ScreenQuad* screenQuad = nullptr;

	int currentWidth;
	int currentHeight;
};