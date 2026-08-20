#ifndef TIMELINE_PREVIEW_H
#define TIMELINE_PREVIEW_H

#include "Core/Animation/Renderer/Renderer.h"

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