#ifndef TIMELINE_PREVIEW_H
#define TIMELINE_PREVIEW_H

#include "Core/Animation/Renderer/Renderer.h"
#include "Core/Animation/Widgets/Colors/Widgets.Colors.h"
#include "Utils/ImGui/Utils.ImGui.h"

#include "Utils/Timer/Timer.Framerate.h"
#include "Utils/TransformScaled/Transform.Scaled.h"

#include <Utils/OpenGL/Texture/Texture.h>
#include <Utils/OpenGL/PixelReadback/PixelReadback.h>

#include "Core/DebugOverlay/DebugOverlay.h"



class Timeline_Preview
{
public:
	Timeline_Preview(FPS_Timer* deltaTime, RendererAnimationScene* renderer);
	~Timeline_Preview();

	void Draw();
	void Update();

private:
	FPS_Timer* deltaTime;
	RendererAnimationScene* renderer;

	TransformScaled horizontal, vertical;

	bool firstCentered = false;

	bool mouseIsDraging = false;

	Texture* temp;
	PixelReadback pixelPicker;

};




#endif