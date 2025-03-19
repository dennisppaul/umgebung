#include <fstream>
#include <sstream>
#include <iostream>

#include "UmgebungFunctionsAdditional.h"
#include "PShader.h"

PShader::PShader() : programID(0) {}

PShader::~PShader() {
    if (programID) {
        glDeleteProgram(programID);
    }
}

bool PShader::load(const std::string& vertex_code, const std::string& fragment_code, const std::string& geometry_code) {

    const GLuint vertexShader   = compileShader(vertex_code, GL_VERTEX_SHADER);
    const GLuint fragmentShader = compileShader(fragment_code, GL_FRAGMENT_SHADER);
    GLuint       geometryShader = 0;

    if (!geometry_code.empty()) {
        umgebung::warning("geometry shader not empty");
        geometryShader = compileShader(geometry_code, GL_GEOMETRY_SHADER);
    }

    programID = glCreateProgram();
    glAttachShader(programID, vertexShader);
    glAttachShader(programID, fragmentShader);
    if (!geometry_code.empty()) {
        glAttachShader(programID, geometryShader);
    }

    glLinkProgram(programID);
    checkLinkErrors(programID);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    if (!geometry_code.empty()) {
        glDeleteShader(geometryShader);
    }

    return true;
}

void PShader::use() const {
    glUseProgram(programID);
}

void PShader::unuse() {
    glUseProgram(0);
}

GLuint PShader::compileShader(const std::string& source, GLenum type) {
    const GLuint shader = glCreateShader(type);
    const char*  src    = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);
    checkCompileErrors(shader, type);
    return shader;
}

void PShader::checkCompileErrors(GLuint shader, GLenum type) {
    GLint  success;
    GLchar infoLog[1024];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
        std::cerr << "Shader Compilation Error (Type: " << type << ")\n"
                  << infoLog << std::endl;
    }
}

void PShader::checkLinkErrors(GLuint program) {
    GLint  success;
    GLchar infoLog[1024];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 1024, nullptr, infoLog);
        std::cerr << "Shader Linking Error\n"
                  << infoLog << std::endl;
    }
}

GLint PShader::getUniformLocation(const std::string& name) {
    if (uniformLocations.find(name) == uniformLocations.end()) {
        uniformLocations[name] = glGetUniformLocation(programID, name.c_str());
    }
    return uniformLocations[name];
}

void PShader::set_uniform(const std::string& name, int value) {
    glUniform1i(getUniformLocation(name), value);
}

void PShader::set_uniform(const std::string& name, float value) {
    glUniform1f(getUniformLocation(name), value);
}

void PShader::set_uniform(const std::string& name, const glm::vec2& value) {
    glUniform2fv(getUniformLocation(name), 1, &value[0]);
}

void PShader::set_uniform(const std::string& name, const glm::vec3& value) {
    glUniform3fv(getUniformLocation(name), 1, &value[0]);
}

void PShader::set_uniform(const std::string& name, const glm::vec4& value) {
    glUniform4fv(getUniformLocation(name), 1, &value[0]);
}

void PShader::set_uniform(const std::string& name, const glm::mat4& value) {
    glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, &value[0][0]);
}