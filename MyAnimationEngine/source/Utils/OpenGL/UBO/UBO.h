#ifndef UBO_H
#define UBO_H

#include "Utils/OpenGL/Shaders/Shader.h"
#include "GLFW/glfw3.h"

#include <string>
#include <iostream>

template <typename T>
class UBO {
public:

	UBO(const std::string& name);
	UBO(const char* name);
	UBO();
	~UBO();

	int GetID();
	std::string GetName();
	void SetName(const std::string& name);

	void BindUBO(const GLuint& bindingPoint);
	void BindProgramm(const Shader& programm);

	void Update(const T& data);

private:
	std::string name;
	GLuint ID;
	GLuint bindingPoint;

	void Constructor(const std::string& name);

};

template <typename T>
UBO<T>::UBO(const std::string& name) {
	Constructor(name);
}

template <typename T>
UBO<T>::UBO(const char* name) {
	Constructor(name);
}

template <typename T>
UBO<T>::UBO() {
	Constructor("");
}

template <typename T>
UBO<T>::~UBO() {
	glDeleteBuffers(1, &ID);
}

template <typename T>
int UBO<T>::GetID() {
	return ID;
}

template <typename T>
std::string UBO<T>::GetName() {
	return name;
}

template <typename T>
void UBO<T>::SetName(const std::string& name) {
	this->name = name;
}

template <typename T>
void UBO<T>::BindUBO(const GLuint& bindingPoint) {
	this->bindingPoint = bindingPoint;
	glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, ID);
}

template <typename T>
void UBO<T>::BindProgramm(const Shader& programm) {
	GLuint blockIndex = glGetUniformBlockIndex(programm.ID, name.c_str());
	glUniformBlockBinding(programm.ID, blockIndex, bindingPoint);
}

template <typename T>
void UBO<T>::Update(const T& data) {
	glBindBuffer(GL_UNIFORM_BUFFER, ID);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(T), &data);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

template <typename T>
void UBO<T>::Constructor(const std::string& name) {
	this->name = name;

	glGenBuffers(1, &ID);
	glBindBuffer(GL_UNIFORM_BUFFER, ID);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(T), nullptr, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

#ifndef _DEBUG

	std::cout << "UBO [" << name << "]\n";
	std::cout << "\t\tsize: " << sizeof(T) << std::endl;
	std::cout << "\t\tID:   " << ID << std::endl;

#endif // !_DEBUG

}


#endif