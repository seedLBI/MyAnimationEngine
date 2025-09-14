#pragma once
#include "RenderObjects/Quad/Quad.h"
#include "QuadAtlasMSDF.Data.h"

class QuadAtlasMSDF_InstancedRender : public Quad, public RenderableObject {
public:
	QuadAtlasMSDF_InstancedRender();
	~QuadAtlasMSDF_InstancedRender();

	void UpdateInstanceData(QuadAtlasMSDF_Data* data, size_t count);

	void Init() override;
	void Render() override;
	void Update() override;

private:
	GLuint VBO_instanced;
	int countInstance = 0;
};