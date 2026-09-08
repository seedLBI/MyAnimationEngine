#ifndef TIMELINE_PREVIEW_H
#define TIMELINE_PREVIEW_H

#include "Core/Animation/Renderer/Renderer.h"
#include "Core/Animation/Widgets/Colors/Widgets.Colors.h"
#include "Utils/ImGui/Utils.ImGui.h"

class Timeline_Preview
{
public:
	Timeline_Preview(RendererAnimationScene* renderer);
	~Timeline_Preview();

	void Draw();
	void Update();

private:
	RendererAnimationScene* renderer;

};




#endif