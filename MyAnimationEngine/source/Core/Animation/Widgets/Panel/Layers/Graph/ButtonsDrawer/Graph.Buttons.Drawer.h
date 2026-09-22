#ifndef GRAPH_BUTTONS_DRAWE_H
#define GRAPH_BUTTONS_DRAWE_H

#include "ThirdParty/ImGui/imgui.h"
#include "ThirdParty/ImGui/imgui_internal.h"

#include "Core/Animation/Widgets/Panel/Context/Timeline.Context.h"

#include "Utils/ImGui/Utils.ImGui.h"
#include <Utils/OpenGL/Texture/Texture.h>
#include <magic_enum/magic_enum.hpp>

#include <functional>


enum GraphButtonsType {
	BTN_GRAPH_MAKE_KEYFRAME_LINEAR,
	BTN_GRAPH_MAKE_KEYFRAME_BINARY,
	BTN_GRAPH_MAKE_KEYFRAME_BEZIER,
	BTN_GRAPH_MAKE_KEYFRAME_AUTOBEZIER
};

class GraphButtonsDrawer
{
public:
	GraphButtonsDrawer(TimelineContext* ctx);
	~GraphButtonsDrawer();

	void Draw(const ImVec2& p_min, const ImVec2& p_max);

private:
	TimelineContext* ctx;

	struct Button {
		std::shared_ptr<Texture> texture;
		void(GraphButtonsDrawer::* action)();
	};

	enum LayoutType {
		SEPARATOR,
		SPACE
	};

	std::vector<std::variant<Button, LayoutType>> layout;
	

	std::vector<std::unique_ptr<Texture>> textures;

	void InitButtons();

	std::shared_ptr<Texture> MakeTexture(GraphButtonsType type, const std::string& fullpath);

	void Button_Make_Selected_Keyframes_Linear();
	void Button_Make_Selected_Keyframes_Binary();
	void Button_Make_Selected_Keyframes_Bezier();
	void Button_Make_Selected_Keyframes_AutoBezier();


	void ChangeTypeKeyframe(TypeKeyframe type);

};


#endif
