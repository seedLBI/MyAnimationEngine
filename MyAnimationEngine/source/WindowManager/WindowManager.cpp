#include "WindowManager.h"
#include <iostream>

WindowManager& WindowManager::getInstance() {
	static WindowManager window;
	return window;
}

GLFWwindow* WindowManager::GetHandle() {
	return handleWindow;
}


void WindowManager::InitGlew() {
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		std::cout << "Failed to initialize GLEW" << std::endl;
		exit(-2);
	}
	glEnable(GL_BLEND);
	glBlendFuncSeparate(GL_ONE, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	glBlendEquation(GL_FUNC_ADD);
}

WindowManager::WindowManager() {
#ifdef _DEBUG
	std::cout << "WindowManager::WindowManager()\n";
#endif
	handleWindow = glfwCreateWindow(1280, 720, "Default window title", NULL, NULL);
	if (handleWindow == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		exit(-1);
	}
	glfwMakeContextCurrent(handleWindow);

	InitGlew();
}

WindowManager::~WindowManager() {
#ifdef _DEBUG
	std::cout << "WindowManager::~WindowManager()\n";
#endif
	glfwDestroyWindow(handleWindow);
}


void WindowManager::SetTitle(const std::string& title) {
	glfwSetWindowTitle(handleWindow, title.c_str());

}
void WindowManager::SetPosition(const glm::vec2& pos) {
	glfwSetWindowPos(handleWindow, static_cast<int>(pos.x), static_cast<int>(pos.y));
}
void WindowManager::SetSize(const glm::vec2& size) {
	glfwSetWindowSize(handleWindow, static_cast<int>(size.x), static_cast<int>(size.y));
	glViewport(0, 0, size.x, size.y);
}

glm::vec2 WindowManager::GetSize() {
	int width, height;
	glfwGetWindowSize(handleWindow, &width, &height);
	return glm::vec2(width, height);
}
glm::vec2 WindowManager::GetPosition() {
	int posX = 0, posY = 0;
	glfwGetWindowPos(handleWindow, &posX, &posY);
	return glm::vec2(posX, posY);
}
std::string WindowManager::GetTitle() {
	return glfwGetWindowTitle(handleWindow);
}

void WindowManager::Centerize() {
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);

	int currentWidth, currentHeight;
	glfwGetWindowSize(handleWindow, &currentWidth, &currentHeight);

	glfwSetWindowPos(handleWindow, (mode->width - currentWidth) / 2, (mode->height - currentHeight) / 2);
}


void WindowManager::Maximize() {
	glfwMaximizeWindow(handleWindow);
}
void WindowManager::Hide() {
	glfwHideWindow(handleWindow);
}

void WindowManager::Close() {
	glfwSetWindowShouldClose(handleWindow, 1);
}

bool WindowManager::IsShouldClose() {
	return glfwWindowShouldClose(handleWindow);
}

