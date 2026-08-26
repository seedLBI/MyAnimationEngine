#include "CursorManger.h"

CursorManager& CursorManager::getInstance() {
	static CursorManager instance;
	return instance;
}

CursorManager::CursorManager() {
	LoadCursors();
}
CursorManager::~CursorManager() {
	//FreeCursors();
}

void CursorManager::SetCursor(const TypeCursor& cursor) {
	current = cursor;
}

void CursorManager::Update() {
	static TypeCursor typeLastCursor = TypeCursor::POINTER;

	if (typeLastCursor != current) {
		glfwSetCursor(WindowManager::getInstance().GetHandle(), cursors[(int)current]);
	}

	typeLastCursor = current;
}


GLFWcursor* CursorManager::LoadImageCursorFromFile(const std::string& path, int x_offset, int y_offset) {
	int w, h, c;

	stbi_set_flip_vertically_on_load(false);
	uint8_t* pixels = stbi_load(path.c_str(), &w, &h, &c, 4);

	GLFWimage image;
	image.width = w;
	image.height = h;
	image.pixels = pixels;

	GLFWcursor* cursor = glfwCreateCursor(&image, x_offset, y_offset);

	stbi_image_free(pixels);

	return cursor;
}

void CursorManager::LoadCursors() {
	cursors.resize(9);

	cursors[(int)TypeCursor::POINTER]			= LoadImageCursorFromFile("resources/cursors/pointer_c_shaded.png",		4,	2);
	cursors[(int)TypeCursor::POINT]				= LoadImageCursorFromFile("resources/cursors/hand_thin_point.png",		6,	4);
	cursors[(int)TypeCursor::HAND_OPEN]			= LoadImageCursorFromFile("resources/cursors/hand_thin_open.png",		8,	6);
	cursors[(int)TypeCursor::HAND_CLOSED]		= LoadImageCursorFromFile("resources/cursors/hand_thin_closed.png",		8,	6);
	cursors[(int)TypeCursor::RESIZE_HORIZONTAL] = LoadImageCursorFromFile("resources/cursors/resize_b_horizontal.png",	16,	16);
	cursors[(int)TypeCursor::RESIZE_VERTICAL]	= LoadImageCursorFromFile("resources/cursors/resize_b_vertical.png",	16, 16);
	cursors[(int)TypeCursor::TEXT]				= LoadImageCursorFromFile("resources/cursors/bracket_b_vertical.png",	16, 16);
	cursors[(int)TypeCursor::DISABLED]			= LoadImageCursorFromFile("resources/cursors/disabled.png",				16, 16);
	cursors[(int)TypeCursor::COPY]				= LoadImageCursorFromFile("resources/cursors/cursor_copy.png",			0, 0);
	
	glfwSetCursor(WindowManager::getInstance().GetHandle(), cursors[(int)current]);
}

void CursorManager::FreeCursors() {
	for (GLFWcursor* cursor : cursors)
		glfwDestroyCursor(cursor);
	cursors.clear();
}