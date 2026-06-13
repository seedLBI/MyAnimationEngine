#include "Shader.h"

Shader::Shader(const std::string& Name, const char* vertexCode, const char* fragmentCode) : Name(Name) {
    const char* vShaderCode = vertexCode;
    const char* fShaderCode = fragmentCode;

    codeVertex = vertexCode;
    codeFragment = fragmentCode;

    unsigned int vertex, fragment;

    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    checkCompileErrors(vertex, TypeErrorShader::VERTEX);

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, TypeErrorShader::FRAGMENT);

    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    checkCompileErrors(ID, TypeErrorShader::PROGRAM);

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

Shader::Shader(const std::string& Name, std::string vertexPath, std::string fragmentPath) : Name(Name)
{
    std::cout << "Trying compile shader programm\n\t[" << vertexPath << "]\n\t[" << fragmentPath << "]\n";

    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;

    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try
    {
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;

        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();

        vShaderFile.close();
        fShaderFile.close();

        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch (std::ifstream::failure& e) {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    codeVertex = vShaderCode;
    codeFragment = fShaderCode;


    unsigned int vertex, fragment;

    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    checkCompileErrors(vertex, TypeErrorShader::VERTEX);

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, TypeErrorShader::FRAGMENT);

    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    checkCompileErrors(ID, TypeErrorShader::PROGRAM);

    glDeleteShader(vertex);
    glDeleteShader(fragment);


    std::cout << "CORRECT shader\n";
}

Shader::Shader(const std::string& Name, const char* vertexCode, const char* geometryCode, const char* fragmentCode) : Name(Name)
{
    const char* vShaderCode = vertexCode;
    const char* gShaderCode = geometryCode;
    const char* fShaderCode = fragmentCode;

    codeVertex = vertexCode;
    codeFragment = fragmentCode;
    codeGeometry = geometryCode;

    unsigned int vertex, geometry, fragment;

    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    checkCompileErrors(vertex, TypeErrorShader::VERTEX);

    geometry = glCreateShader(GL_GEOMETRY_SHADER);
    glShaderSource(geometry, 1, &gShaderCode, NULL);
    glCompileShader(geometry);
    checkCompileErrors(geometry, TypeErrorShader::GEOMETRY);

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, TypeErrorShader::FRAGMENT);

    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, geometry);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    checkCompileErrors(ID, TypeErrorShader::PROGRAM);

    glDeleteShader(vertex);
    glDeleteShader(geometry);
    glDeleteShader(fragment);
}

Shader::Shader(const std::string& Name, std::string vertexPath, std::string geometryPath, std::string fragmentPath) : Name(Name)
{
    std::cout << "Trying compile shader programm\n\t[" << vertexPath << "]\n\t[" << fragmentPath << "]\n";

    std::string vertexCode;
    std::string geometryCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream gShaderFile;
    std::ifstream fShaderFile;

    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        vShaderFile.open(vertexPath);
        gShaderFile.open(geometryPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, gShaderStream, fShaderStream;

        vShaderStream << vShaderFile.rdbuf();
        gShaderStream << gShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();

        vShaderFile.close();
        gShaderFile.close();
        fShaderFile.close();

        vertexCode = vShaderStream.str();
        geometryCode = gShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch (std::ifstream::failure& e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }
    const char* vShaderCode = vertexCode.c_str();
    const char* gShaderCode = geometryCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    codeVertex = vShaderCode;
    codeFragment = gShaderCode;
    codeGeometry = fShaderCode;

    unsigned int vertex, geometry, fragment;

    std::cout << "create vertex\n";
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    checkCompileErrors(vertex, TypeErrorShader::VERTEX);

    std::cout << "create geometry\n";
    geometry = glCreateShader(GL_GEOMETRY_SHADER);
    glShaderSource(geometry, 1, &gShaderCode, NULL);
    glCompileShader(geometry);
    checkCompileErrors(geometry, TypeErrorShader::GEOMETRY);

    std::cout << "create fragment\n";
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, TypeErrorShader::FRAGMENT);

    std::cout << "create Program\n";
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, geometry);
    glAttachShader(ID, fragment);

    std::cout << "link Program\n";
    glLinkProgram(ID);
    checkCompileErrors(ID, TypeErrorShader::PROGRAM);

    std::cout << "delete shaders\n";
    glDeleteShader(vertex);
    glDeleteShader(geometry);
    glDeleteShader(fragment);

    std::cout << "end.\n";
}


Shader::Shader() {

}

Shader::~Shader() {
    glDeleteProgram(ID);
}

// Активация шейдера
void Shader::use()
{
    glUseProgram(ID);
}


void Shader::setBool(const std::string& name, bool value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}
void Shader::setInt(const std::string& name, int value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}
void Shader::setFloat(const std::string& name, float value) const
{
    if (glGetUniformLocation(ID, name.c_str()) <= -1)
    {
        //std::cout << name << " not founded((((" << std::endl;
    }
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}
void Shader::setFloat(GLuint ID, float value) const
{
    glUniform1f(ID, value);
}
void Shader::set2Float(const std::string& name, float value1, float value2) const
{
    glUniform2f(glGetUniformLocation(ID, name.c_str()), value1, value2);
}
void Shader::set2Int(const std::string& name, int value1, int value2) const
{
    glUniform2i(glGetUniformLocation(ID, name.c_str()), value1, value2);
}
void Shader::set3Float(const std::string& name, float value1, float value2, float value3) const
{
    glUniform3f(glGetUniformLocation(ID, name.c_str()), value1, value2, value3);
}
void Shader::set3Float(const std::string& name, glm::vec3 value) const
{
    glUniform3f(glGetUniformLocation(ID, name.c_str()), value.x, value.y, value.z);
}
void Shader::set3Float(GLuint ID, glm::vec3 value) const
{
    glUniform3f(ID, value.x, value.y, value.z);
}
void Shader::set4Float(const std::string& name, float v1, float v2, float v3, float v4) {
    glUniform4f(glGetUniformLocation(ID, name.c_str()), v1, v2, v3, v4);
}
void Shader::set4Float(const std::string& name, glm::vec4 values) {
    glUniform4f(glGetUniformLocation(ID, name.c_str()), values.x, values.y, values.z, values.w);
}
void Shader::set4FloatWithInfo(const std::string& name, glm::vec4 values) {
    glUniform4f(glGetUniformLocation(ID, name.c_str()), values.x, values.y, values.z, values.w);
}
void Shader::setMat4(const std::string& name, const glm::mat4& mat) const {
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
void Shader::setMat4(GLuint ID, const glm::mat4& mat) const
{
    glUniformMatrix4fv(ID, 1, GL_FALSE, &mat[0][0]);
}


void Shader::bindUniformBlock(const std::string& name, int number_UBO) {
    unsigned int name_index = glGetUniformBlockIndex(ID, name.c_str());
#ifdef WITH_DEBUG_OUTPUT
    std::cout << name << " |in shader have id| " << name_index << std::endl;
#endif
    glUniformBlockBinding(ID, name_index, number_UBO);
}

std::string Shader::getStrTypeError(const TypeErrorShader& type) {
    switch (type) {
    case Shader::VERTEX:
        return "VERTEX";
        break;
    case Shader::FRAGMENT:
        return "FRAGMENT";
        break;
    case Shader::GEOMETRY:
        return "GEOMETRY";
        break;
    case Shader::PROGRAM:
        return "PROGRAM";
        break;
    default:
        return "UNKNOWN_TYPE_ERORR";
    }
}
std::string& Shader::getCodeByType(const TypeErrorShader& type) {
    switch (type) {
    case Shader::VERTEX:
        return codeVertex;
        break;
    case Shader::FRAGMENT:
        return codeFragment;
        break;
    case Shader::GEOMETRY:
        return codeGeometry;
        break;
    case Shader::PROGRAM:
        return codeVertex;
        break;
    default:
        return codeVertex;
    }
}



// вспомогательная: разбить строку на строки
static std::vector<std::string> splitLines(const std::string& s) {
    std::vector<std::string> out;
    std::istringstream iss(s);
    std::string line;
    while (std::getline(iss, line)) out.push_back(line);
    return out;
}




// функция подсветки подстроки в строке (возвращает строку с ANSI-обёрткой)
static std::string highlightSubstring(const std::string& line, size_t pos, size_t len) {
    if (pos == std::string::npos || len == 0) return line;
    std::string out;
    out.reserve(line.size() + 20);
    out += line.substr(0, pos);
    out += ANSI_BG_RED;
    out += ANSI_FG_WHITE;
    out += line.substr(pos, len);
    out += ANSI_RESET;
    out += line.substr(pos + len);
    return out;
}

// подсветить слова в сообщении (каждое слово из highlightWords)
static std::string highlightWordsInMessage(const std::string& msg, const std::vector<std::string>& words) {
    if (words.empty()) return msg;
    std::string res = msg;
    // простой однопроходный поиск — учитываем возможные пересечения
    for (const auto& w : words) {
        if (w.empty()) continue;
        std::string lw = w; // case-sensitive; при желании можно lower-case и msg -> lower-case копию
        size_t pos = 0;
        while ((pos = res.find(lw, pos)) != std::string::npos) {
            // вставляем ANSI: replace with highlighted fragment
            std::string before = res.substr(0, pos);
            std::string match = res.substr(pos, lw.size());
            std::string after = res.substr(pos + lw.size());
            res = before + ANSI_BG_RED + ANSI_FG_WHITE + match + ANSI_RESET + after;
            pos += strlen(ANSI_BG_RED) + strlen(ANSI_FG_WHITE) + lw.size() + strlen(ANSI_RESET); // переехать дальше
        }
    }
    return res;
}



void Shader::printParsedInfoLog(const std::string& infoLog, const TypeErrorShader& type,
    const std::vector<std::string>& highlightWords)
{
    std::vector<std::string> lines = splitLines(infoLog);

    // Получаем исходный код по типу (используется ваш поля класса)
    const std::string& source = getCodeByType(type);
    std::vector<std::string> srcLines = splitLines(source);

    // Популярные форматы логов: "ERROR: 0:25: 'token' : message"
    // составим несколько regex-паттернов для большей совместимости
    std::regex re1(R"(^\s*(ERROR|WARNING)\s*:\s*(\d+)\s*:\s*(\d+)\s*:\s*'([^']*)'\s*:\s*(.*)$)");
    std::regex re2(R"(^\s*(ERROR|WARNING)\s*:\s*(\d+)\s*:\s*(\d+)\s*:\s*(.*)$)"); // без '' token
    std::smatch m;

    for (const auto& l : lines) {
        std::string trimmed = l;
        // пробуем вытащить поля
        std::string level, fileIdxStr, lineNumStr, token, message;
        bool parsed = false;
        if (std::regex_match(trimmed, m, re1)) {
            level = m[1]; fileIdxStr = m[2]; lineNumStr = m[3]; token = m[4]; message = m[5];
            parsed = true;
        }
        else if (std::regex_match(trimmed, m, re2)) {
            level = m[1]; fileIdxStr = m[2]; lineNumStr = m[3]; token = ""; message = m[4];
            parsed = true;
        }

        if (!parsed) {
            // если не распарсилось — просто выводим строку (с возможным highlight ключевых слов)
            std::cout << highlightWordsInMessage(trimmed, highlightWords) << std::endl;
            continue;
        }

        // конвертируем номер строки
        int lineNumber = 0;
        try { lineNumber = std::stoi(lineNumStr); }
        catch (...) { lineNumber = 0; }

        std::cout << "SHADER_NAME: " << Name << std::endl;
        std::cout << level << " (type " << getStrTypeError(type) << ") at line " << lineNumber << ": "
            << highlightWordsInMessage(message, highlightWords) << std::endl;

        // печатаем контекст: предыдущая, текущая, следующая строки (если есть)
        int idx = lineNumber - 1; // user lines usually 1-based
        int start = std::max(0, idx - 1);
        int end = std::min((int)srcLines.size() - 1, idx + 1);

        for (int i = start; i <= end; ++i) {
            // формируем номер строки с выравниванием
            std::ostringstream num;
            num << (i + 1);
            std::string lineno = num.str();
            std::string codeLine = srcLines.size() ? srcLines[i] : std::string();

            if (i == idx) {
                // пытаемся найти token в этой строке
                size_t pos = std::string::npos;
                size_t tlen = 0;
                if (!token.empty()) {
                    pos = codeLine.find(token);
                    if (pos != std::string::npos) tlen = token.size();
                }
                // если token не указан или не найден, попробуем подсказать позицию:
                if (pos == std::string::npos) {
                    // попытаемся искать слово из сообщения в коде
                    std::vector<std::string> words;
                    // извлечь слова из message (простая токенизация)
                    std::istringstream iss(message);
                    std::string w;
                    while (iss >> w) {
                        // очистим от знаков препинания в начале/конце
                        while (!w.empty() && ispunct(w.front())) w.erase(w.begin());
                        while (!w.empty() && ispunct(w.back())) w.pop_back();
                        if (!w.empty() && w.size() > 1) words.push_back(w);
                    }
                    for (auto& pw : words) {
                        size_t p = codeLine.find(pw);
                        if (p != std::string::npos) { pos = p; tlen = pw.size(); break; }
                    }
                }

                // печатаем саму строку; если нашли pos — подсвечиваем
                if (pos != std::string::npos && tlen > 0) {
                    std::string highlighted = highlightSubstring(codeLine, pos, tlen);
                    std::cout << " " << std::string(4 - (int)lineno.size(), ' ') << lineno << " | " << highlighted << std::endl;
                    // и печатаем caret/строку подчёркивания под словом (чтобы было видно позицию точно)
                    std::string caretLine;
                    // подсчитываем видимую ширину до pos — но учтём ANSI-сбросы ещё не вставлены (мы подсветили уже),
                    // проще: нарисуем caret базируясь на исходной строке (до подсветки)
                    caretLine = std::string(6 + pos, ' ') + std::string(tlen, '^'); // 6 = отступ " {sp}NNN | "
                    std::cout << caretLine << std::endl;
                }
                else {
                    // нет точной позиции — просто выводим строку и подчёркиваем начало
                    std::cout << " " << std::string(4 - (int)lineno.size(), ' ') << lineno << " | " << codeLine << std::endl;
                }
            }
            else {
                // обычная строка контекста
                std::cout << " " << std::string(4 - (int)lineno.size(), ' ') << lineno << " | " << codeLine << std::endl;
            }
        }

        std::cout << " -- --------------------------------------------------- -- " << std::endl;
    }
}

// Пример интеграции в checkCompileErrors (фрагмент — замените вывод infoLog на вызов printParsedInfoLog)
void Shader::checkCompileErrors(unsigned int shader, const TypeErrorShader& type)
{
    int success = 0;
    char infoLog[4096];
    if (type != TypeErrorShader::PROGRAM)
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 4096, NULL, infoLog);
            // вместо простого вывода: распарсим и красиво покажем
            printParsedInfoLog(std::string(infoLog), type, std::vector<std::string>{"qualifiers", "interpolation"});
            exit(0);
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader, 4096, NULL, infoLog);
            printParsedInfoLog(std::string(infoLog), type, {});
            exit(0);
        }
    }
}
