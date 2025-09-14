#ifndef RENDERABLE_OBJECT_H
#define RENDERABLE_OBJECT_H


class RenderableObject {
public:
	RenderableObject() = default;
	~RenderableObject() = default;
	void virtual Init();
	void virtual Render();
	void virtual Update();
};




#endif