#include "Window.h"

#include <assert.h>
#include "Camera.h"

//REMOVE
#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

Window::Window(int width, int height, std::string name)
{
	//Init glfw with OpenGL 3.3 in Core Profile mode
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

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
}

Window::~Window()
{
	glfwTerminate();
}

void Window::BeginFrame()
{
	//Update camera aspect ratio (REPLACE: It may be possible to do this with callback, which would be better)
	int width;
	int height;
	glfwGetWindowSize(window, &width, &height);
	mainCamera->SetAspectRatio((float)width / (float)height);

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

glm::vec2 Window::GetCursorPos() const
{
	double x;
	double y;
	glfwGetCursorPos(window, &x, &y);
	return glm::vec2(x, y);
}

void Window::SetMainCamera(Camera* camera)
{
	mainCamera = camera;
}
