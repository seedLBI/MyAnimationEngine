#include "QuadAtlasMSDF.InstancedRender.h"

#ifdef _DEBUG
#include <iostream>
#endif

QuadAtlasMSDF_InstancedRender::QuadAtlasMSDF_InstancedRender() : Quad() {
#ifdef _DEBUG
	std::cout << "QuadAtlasMSDF_InstancedRender::QuadAtlasMSDF_InstancedRender() : Quad()\n";
#endif
	Init();
}

QuadAtlasMSDF_InstancedRender::~QuadAtlasMSDF_InstancedRender() {
#ifdef _DEBUG
	std::cout << "QuadAtlasMSDF_InstancedRender::~QuadAtlasMSDF_InstancedRender()\n";
#endif

	glDeleteBuffers(1, &VBO_instanced);
}

void QuadAtlasMSDF_InstancedRender::UpdateInstanceData(QuadAtlasMSDF_Data* data, size_t count) {

#ifdef _DEBUG
	std::cout << " QuadAtlasMSDF_InstancedRender::UpdateInstanceData(QuadAtlasMSDF_Data* data, size_t count)\n\tVBO_instanced: " << VBO_instanced << "\n";
#endif

	glBindBuffer(GL_ARRAY_BUFFER, VBO_instanced);
	glBufferData(GL_ARRAY_BUFFER, count * sizeof(QuadAtlasMSDF_Data), data, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	countInstance = count;
}

void QuadAtlasMSDF_InstancedRender::Init() {
#ifdef _DEBUG
	std::cout << "void QuadAtlasMSDF_Instanced::Init()\n";
#endif

	//QuadAtlasMSDF_Data

	glBindVertexArray(VAO);
	glGenBuffers(1, &VBO_instanced);
#ifdef _DEBUG
	std::cout << "\tVBO_instanced: " << VBO_instanced << "\n";
#endif
	glBindBuffer(GL_ARRAY_BUFFER, VBO_instanced);
	glBufferData(GL_ARRAY_BUFFER, sizeof(QuadAtlasMSDF_Data), nullptr, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(2); // AtlasUV (vec4)[4*float]
	glVertexAttribDivisor(2, 1);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(QuadAtlasMSDF_Data), (void*)0);

	glEnableVertexAttribArray(3); // Thikness text (float)
	glVertexAttribDivisor(3, 1);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(QuadAtlasMSDF_Data), (void*)(4 * sizeof(float)));

	glEnableVertexAttribArray(4); // Thikness outline (float)
	glVertexAttribDivisor(4, 1);
	glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(QuadAtlasMSDF_Data), (void*)(5 * sizeof(float)));

	glEnableVertexAttribArray(5); // ColorText (vec4)[4*float]
	glVertexAttribDivisor(5, 1);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(QuadAtlasMSDF_Data), (void*)(6 * sizeof(float)));

	glEnableVertexAttribArray(6); // ColorOutline (vec4)[4*float]
	glVertexAttribDivisor(6, 1);
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(QuadAtlasMSDF_Data), (void*)(10 * sizeof(float)));


	glEnableVertexAttribArray(7); // MatrixTransform (mat4)[16*float] [4*float]
	glVertexAttribDivisor(7, 1);
	glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(QuadAtlasMSDF_Data), (void*)((14 + 4 * 0) * sizeof(float)));

	glEnableVertexAttribArray(8); // MatrixTransform (mat4)[16*float] [8*float]
	glVertexAttribDivisor(8, 1);
	glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(QuadAtlasMSDF_Data), (void*)((14 + 4 * 1) * sizeof(float)));

	glEnableVertexAttribArray(9); // MatrixTransform (mat4)[16*float] [12*float]
	glVertexAttribDivisor(9, 1);
	glVertexAttribPointer(9, 4, GL_FLOAT, GL_FALSE, sizeof(QuadAtlasMSDF_Data), (void*)((14 + 4 * 2) * sizeof(float)));

	glEnableVertexAttribArray(10); // MatrixTransform (mat4)[16*float] [16*float]
	glVertexAttribDivisor(10, 1);
	glVertexAttribPointer(10, 4, GL_FLOAT, GL_FALSE, sizeof(QuadAtlasMSDF_Data), (void*)((14 + 4 * 3) * sizeof(float)));


	glEnableVertexAttribArray(11); // Prev_MatrixTransform (mat4)[16*float] [4*float]
	glVertexAttribDivisor(11, 1);
	glVertexAttribPointer(11, 4, GL_FLOAT, GL_FALSE, sizeof(QuadAtlasMSDF_Data), (void*)((14 + 4 * 4) * sizeof(float)));

	glEnableVertexAttribArray(12); // Prev_MatrixTransform (mat4)[16*float] [8*float]
	glVertexAttribDivisor(12, 1);
	glVertexAttribPointer(12, 4, GL_FLOAT, GL_FALSE, sizeof(QuadAtlasMSDF_Data), (void*)((14 + 4 * 5) * sizeof(float)));

	glEnableVertexAttribArray(13); // Prev_MatrixTransform (mat4)[16*float] [12*float]
	glVertexAttribDivisor(13, 1);
	glVertexAttribPointer(13, 4, GL_FLOAT, GL_FALSE, sizeof(QuadAtlasMSDF_Data), (void*)((14 + 4 * 6) * sizeof(float)));

	glEnableVertexAttribArray(14); // Prev_MatrixTransform (mat4)[16*float] [16*float]
	glVertexAttribDivisor(14, 1);
	glVertexAttribPointer(14, 4, GL_FLOAT, GL_FALSE, sizeof(QuadAtlasMSDF_Data), (void*)((14 + 4 * 7) * sizeof(float)));


	glBindBuffer(GL_ARRAY_BUFFER, 0);

}
void QuadAtlasMSDF_InstancedRender::Render() {
	glBindVertexArray(VAO);
	glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, countInstance);
}

void QuadAtlasMSDF_InstancedRender::Update() {

}
