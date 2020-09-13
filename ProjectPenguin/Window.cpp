#include "Window.h"

#include <assert.h>
#include <iostream>

#include "Camera.h"
#include "ScreenQuad.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

Window::Window(int width, int height, std::string name)
	:
	currentWidth(width),
	currentHeight(height)
{
	//Init glfw with OpenGL 3.3 in Core Profile mode
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//REPLACE: toggles multi sampling (anti aliasing)
	//glfwWindowHint(GLFW_SAMPLES, 16);

	//Create the actual window
	window = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);
	assert(window);
	glfwMakeContextCurrent(window);

	//Init glad
	auto temp = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	assert(temp);

	//Init viewport with same properties as the window
	glViewport(0, 0, width, height);

	//Make sure that the viewport is automatically resized with the window
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	//Set initial clear color 
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

	//REPLACE: enable multisampling
	//glEnable(GL_MULTISAMPLE);

	//Set blend function for transparency
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Enable backface culling
	glEnable(GL_CULL_FACE);

	//Enable depth testing
	glEnable(GL_DEPTH_TEST);
}

Window::~Window()
{
	glfwTerminate();
}

void Window::BeginFrame()
{
	//Call "ResizeCallback" if window has been resized. REPLACE: Doing this with a real callback would be better
	int newWidth, newHeight;
	glfwGetWindowSize(window, &newWidth, &newHeight);
	if (newWidth != currentWidth || newHeight != currentHeight)
	{
		currentWidth = newWidth;
		currentHeight = newHeight;
		ResizeCallback();
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Window::EndFrame()
{
	glfwPollEvents();
	glfwSwapBuffers(window);
}

void Window::SetTitle(std::string name)
{
	glfwSetWindowTitle(window, name.c_str());
}

void Window::Close()
{
	glfwSetWindowShouldClose(window, true);
}

bool Window::IsClosing() const
{
	return glfwWindowShouldClose(window);
}

bool Window::KeyIsPressed(int key) const
{
	return glfwGetKey(window, key);
}

bool Window::MouseButtonIsPressed(int button) const
{
	return glfwGetMouseButton(window, button);
}

glm::vec2 Window::GetCursorPos() const
{
	double x;
	double y;
	glfwGetCursorPos(window, &x, &y);
	return glm::vec2(x, y);
}

glm::vec2 Window::GetDimensions() const
{
	return glm::vec2((float)currentWidth, (float)currentHeight);
}

int Window::GetWidth() const
{
	return currentWidth;
}

int Window::GetHeight() const
{
	return currentHeight;
}

void Window::SetMainCamera(Camera* camera)
{
	mainCamera = camera;
	ResizeCallback();
}

void Window::SetScreenQuad(ScreenQuad* inScreenQuad)
{
	screenQuad = inScreenQuad;
	ResizeCallback();
}

void Window::ResizeCallback()
{
	if (mainCamera)
	{
		mainCamera->SetAspectRatio((float)currentWidth / (float)currentHeight);
	}

	if (screenQuad)
	{
		screenQuad->UpdateDimensions();
	}

	std::cout << "Window is updated";
}
