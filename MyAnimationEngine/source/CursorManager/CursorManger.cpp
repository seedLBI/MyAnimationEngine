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


GLFWcursor* CursorManager::LoadImageCursorFromFile(const char* path, int x_offset, int y_offset) {
	int w, h, c;

	stbi_set_flip_vertically_on_load(false);
	uint8_t* pixels = stbi_load(path, &w, &h, &c, 4);

	GLFWimage image;
	image.width = w;
	image.height = h;
	image.pixels = pixels;

	GLFWcursor* cursor = glfwCreateCursor(&image, x_offset, y_offset);

	stbi_image_free(pixels);

	return cursor;
}

#include <magic_enum/magic_enum.hpp>

void CursorManager::LoadCursors() {

	struct CursorInfo {
		TypeCursor type;
		const char* filename;
		int x_offset;
		int y_offset;
	};

	const std::string base_path = "resources/images/cursors/";

	const CursorInfo cursors_data[] = {
		{ TypeCursor::POINTER,           "pointer_c_shaded.png",     4,  2 },
		{ TypeCursor::POINT,             "hand_thin_point.png",      6,  4 },
		{ TypeCursor::HAND_OPEN,         "hand_thin_open.png",       8,  6 },
		{ TypeCursor::HAND_CLOSED,       "hand_thin_closed.png",     8,  6 },
		{ TypeCursor::RESIZE_HORIZONTAL, "resize_b_horizontal.png", 16, 16 },
		{ TypeCursor::RESIZE_VERTICAL,   "resize_b_vertical.png",   16, 16 },
		{ TypeCursor::TEXT,              "bracket_b_vertical.png",  16, 16 },
		{ TypeCursor::DISABLED,          "disabled.png",            16, 16 },
		{ TypeCursor::COPY,              "cursor_copy.png",          0,  0 },
		{ TypeCursor::MAGNET,            "magnet.png",              12,  5 }
	};

	constexpr size_t cursor_count = magic_enum::enum_count<TypeCursor>();
	cursors.resize(cursor_count);


	for (const auto& info : cursors_data) {
		std::string full_path = base_path + info.filename;
		cursors[(int)info.type] = LoadImageCursorFromFile(full_path.c_str(), info.x_offset, info.y_offset);
	}

	glfwSetCursor(WindowManager::getInstance().GetHandle(), cursors[(int)current]);
}

void CursorManager::FreeCursors() {
	for (GLFWcursor* cursor : cursors)
		glfwDestroyCursor(cursor);
	cursors.clear();
}