#include "Quad.h"

#ifdef _DEBUG
#include <iostream>
#endif

Quad::Quad() {
#ifdef _DEBUG
	std::cout << "Quad::Quad()\n";
#endif
	Init();
}

Quad::~Quad() {
#ifdef _DEBUG
	std::cout << "Quad::~Quad()\n";
#endif

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}


void Quad::Init() {
#ifdef _DEBUG
	std::cout << "void Quad::Init()\n";
#endif
	float vertices[] = {
		//  pos				texcoord
		-1.f, -1.f, 0.f,    0.f, 0.f, //lb
		-1.f,  1.f, 0.f,    0.f, 1.f, //lt
		1.f,   1.f, 0.f,    1.f, 1.f, //rt
		1.f,  -1.f, 0.f,    1.f, 0.f  //rb
	};

	GLuint indices[] = {
		0,1,2,
		2,3,0
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);


	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// texcoord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
}

void Quad::Render() {
	glBindVertexArray(VAO); 
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void Quad::Update() {

}
