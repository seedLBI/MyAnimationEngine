#ifndef QUAD_H
#define QUAD_H

#define GLEW_STATIC
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
