#ifndef QUAD_H
#define QUAD_H

#ifndef _DEBUG
#define GLEW_STATIC
#endif
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "RenderObjects/RenderableObject.h"

class Quad : public RenderableObject {
public:
	Quad();
	~Quad();
	void Init() override;
	void Render() override;
	void Update() override;

protected:
	GLuint VAO, VBO, EBO;
};




#endif
