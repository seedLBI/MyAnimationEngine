#ifndef SHADER_H
#define SHADER_H
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <sstream>
#include <regex>

#define GLEW_STATIC
#include <GL/glew.h>

#ifdef _WIN32
    #define GLFW_EXPOSE_NATIVE_WIN32
#endif
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

static const char* ANSI_RESET = "\x1b[0m";
static const char* ANSI_BG_RED = "\x1b[41m";
static const char* ANSI_FG_WHITE = "\x1b[97m";
static const char* ANSI_UNDERLINE = "\x1b[4m";

class Shader
{
private:
    std::string Name;
    std::string codeVertex, codeFragment, codeGeometry;

    enum TypeErrorShader {
        VERTEX,
        FRAGMENT,
        GEOMETRY,
        PROGRAM
    };

public:
    unsigned int ID;

    ~Shader();
    Shader();
    Shader(const std::string& Name, const char* vertexCode, const char* fragmentCode);
    Shader(const std::string& Name, std::string vertexPath, std::string fragmentPath);
    Shader(const std::string& Name, const char* vertexCode, const char* geometryCode, const char* fragmentCode);
    Shader(const std::string& Name, std::string vertexPath, std::string geometryPath, std::string fragmentPath);

    void use();
    void bindUniformBlock(const std::string& name, int number_UBO);

    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setFloat(GLuint ID, float value) const;
    void set2Float(const std::string& name, float value1, float value2) const;
    void set2Int(const std::string& name, int value1, int value2) const;
    void set3Float(const std::string& name, float value1, float value2, float value3) const;
    void set3Float(const std::string& name, glm::vec3 value) const;
    void set3Float(GLuint ID, glm::vec3 value) const;
    void set4Float(const std::string& name, float v1, float v2, float v3, float v4);
    void set4Float(const std::string& name, glm::vec4 values);
    void set4FloatWithInfo(const std::string& name, glm::vec4 values);
    void setMat4(const std::string& name, const glm::mat4& mat) const;
    void setMat4(GLuint ID, const glm::mat4& mat) const;

private:
    void checkCompileErrors(unsigned int shader, const TypeErrorShader& type);

    std::string getStrTypeError(const TypeErrorShader& type);
    std::string& getCodeByType(const TypeErrorShader& type);
    void printParsedInfoLog(const std::string& infoLog, const TypeErrorShader& type, const std::vector<std::string>& highlightWords);
};

#endif
