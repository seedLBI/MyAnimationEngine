#ifndef WINDOW_MANAGER_H
#define WINDOW_MANAGER_H

#define GLEW_STATIC
#include <GL/glew.h>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <glm/glm.hpp>
#include <string>


class WindowManager{
public:
	WindowManager(const WindowManager&) = delete;
	WindowManager& operator=(const WindowManager&) = delete;
	static WindowManager& getInstance();

	GLFWwindow* GetHandle();

	void SetTitle(const std::string& title);
	std::string GetTitle();

	void SetPosition(const glm::vec2& pos);
	glm::vec2 GetPosition();

	void SetSize(const glm::vec2& size);
	glm::vec2 GetSize();

	void Centerize();
	void Maximize();
	void Hide();
	void Close();

	bool IsShouldClose();

private:
	WindowManager();

	~WindowManager();


	GLFWwindow* handleWindow;
	void InitGlew();
};




#endif