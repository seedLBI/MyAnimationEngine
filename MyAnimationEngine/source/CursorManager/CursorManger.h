#ifndef CURSOR_MANAGER_H
#define CURSOR_MANAGER_H

#include "ThirdParty/ImGui/imgui.h"
#include "ThirdParty/ImGui/imgui_impl_glfw.h"
#include "ThirdParty/ImGui/imgui_impl_opengl3.h"

#define GLEW_STATIC
#include <GL/glew.h>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>


#include <stb_image.h>

#include <vector>
#include <string>
#include <cstdint>


#include "WindowManager/WindowManager.h"

enum class TypeCursor {
	POINTER,
	POINT,
	HAND_OPEN,
	HAND_CLOSED,
	RESIZE_HORIZONTAL,
	RESIZE_VERTICAL,
	TEXT,
	DISABLED,
	COPY,
	MAGNET
};

class CursorManager {
public:
	CursorManager(const CursorManager&) = delete;
	CursorManager& operator=(const CursorManager&) = delete;

	static CursorManager& getInstance();

	void Update();

	void SetCursor(const TypeCursor& cursor);
	void ForceSetCursor(const TypeCursor& cursor);

private:
	CursorManager();
	~CursorManager();

	std::vector<GLFWcursor*> cursors;
	TypeCursor current = TypeCursor::POINTER;

	GLFWcursor* LoadImageCursorFromFile(const std::string& path, int x_offset = 0, int y_offset = 0);
	void LoadCursors();
	void FreeCursors();
};


#endif