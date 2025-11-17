#include "Shader.hpp"

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath) {
    std::string vertexCode = LoadFile(vertexPath);
    std::string fragmentCode = LoadFile(fragmentPath);

    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, nullptr);
    glCompileShader(vertex);
    CheckCompileErrors(vertex, "VERTEX");

    GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, nullptr);
    glCompileShader(fragment);
    CheckCompileErrors(fragment, "FRAGMENT");

    programId = glCreateProgram();
    glAttachShader(programId, vertex);
    glAttachShader(programId, fragment);
    glLinkProgram(programId);
    CheckCompileErrors(programId, "PROGRAM");

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

Shader::~Shader() {
    glDeleteProgram(programId);
}

// ===== Matrix Uniforms =====
void Shader::SetMat4(const std::string& name, const glm::mat4& mat) const {
    glUniformMatrix4fv(glGetUniformLocation(programId, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::SetMat3(const std::string& name, const glm::mat3& mat) const {
    glUniformMatrix3fv(glGetUniformLocation(programId, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::SetMat2(const std::string& name, const glm::mat2& mat) const {
    glUniformMatrix2fv(glGetUniformLocation(programId, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

// ===== Vector Uniforms =====
void Shader::SetVec4(const std::string& name, const glm::vec4& vec) const {
    glUniform4f(glGetUniformLocation(programId, name.c_str()), vec.x, vec.y, vec.z, vec.w);
}

void Shader::SetVec3(const std::string& name, const glm::vec3& vec) const {
    glUniform3f(glGetUniformLocation(programId, name.c_str()), vec.x, vec.y, vec.z);
}

void Shader::SetVec2(const std::string& name, const glm::vec2& vec) const {
    glUniform2f(glGetUniformLocation(programId, name.c_str()), vec.x, vec.y);
}

// ===== Integer Vector Uniforms =====
void Shader::SetIVec4(const std::string& name, const glm::ivec4& vec) const {
    glUniform4i(glGetUniformLocation(programId, name.c_str()), vec.x, vec.y, vec.z, vec.w);
}

void Shader::SetIVec3(const std::string& name, const glm::ivec3& vec) const {
    glUniform3i(glGetUniformLocation(programId, name.c_str()), vec.x, vec.y, vec.z);
}

void Shader::SetIVec2(const std::string& name, const glm::ivec2& vec) const {
    glUniform2i(glGetUniformLocation(programId, name.c_str()), vec.x, vec.y);
}

// ===== Scalar Uniforms =====
void Shader::SetFloat(const std::string& name, float value) const {
    glUniform1f(glGetUniformLocation(programId, name.c_str()), value);
}

void Shader::SetInt(const std::string& name, int value) const {
    glUniform1i(glGetUniformLocation(programId, name.c_str()), value);
}

void Shader::SetUInt(const std::string& name, unsigned int value) const {
    glUniform1ui(glGetUniformLocation(programId, name.c_str()), value);
}

void Shader::SetBool(const std::string& name, bool value) const {
    glUniform1i(glGetUniformLocation(programId, name.c_str()), static_cast<int>(value));
}

// ===== Array Uniforms =====
void Shader::SetMat4Array(const std::string& name, const glm::mat4* matrices, int count) const {
    glUniformMatrix4fv(glGetUniformLocation(programId, name.c_str()), count, GL_FALSE, &matrices[0][0][0]);
}

void Shader::SetVec3Array(const std::string& name, const glm::vec3* vectors, int count) const {
    glUniform3fv(glGetUniformLocation(programId, name.c_str()), count, &vectors[0][0]);
}

void Shader::SetFloatArray(const std::string& name, const float* values, int count) const {
    glUniform1fv(glGetUniformLocation(programId, name.c_str()), count, values);
}

void Shader::SetIntArray(const std::string& name, const int* values, int count) const {
    glUniform1iv(glGetUniformLocation(programId, name.c_str()), count, values);
}

// ===== Sampler Uniforms =====
void Shader::SetSampler2D(const std::string& name, int textureUnit) const {
    glUniform1i(glGetUniformLocation(programId, name.c_str()), textureUnit);
}

void Shader::SetSamplerCube(const std::string& name, int textureUnit) const {
    glUniform1i(glGetUniformLocation(programId, name.c_str()), textureUnit);
}

// ===== File Loader =====
std::string Shader::LoadFile(const std::string& path) {
    std::ifstream file;
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        file.open(path);
        std::stringstream stream;
        stream << file.rdbuf();
        file.close();
        return stream.str();
    } catch (std::ifstream::failure& e) {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " 
                  << path << " (error: " << e.what() << ")" << std::endl;
        return "";
    }
}

// ===== Reload Shader =====
void Shader::Reload(const std::string& vertexPath, const std::string& fragmentPath) {
    glDeleteProgram(programId); // delete old program

    std::string vertexCode = LoadFile(vertexPath);
    std::string fragmentCode = LoadFile(fragmentPath);

    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, nullptr);
    glCompileShader(vertex);
    CheckCompileErrors(vertex, "VERTEX");

    GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, nullptr);
    glCompileShader(fragment);
    CheckCompileErrors(fragment, "FRAGMENT");

    programId = glCreateProgram();
    glAttachShader(programId, vertex);
    glAttachShader(programId, fragment);
    glLinkProgram(programId);
    CheckCompileErrors(programId, "PROGRAM");

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    std::cout << "[Shader] Reloaded shader program (ID: " << programId << ")" << std::endl;
}

// ===== Error Checking =====
void Shader::CheckCompileErrors(GLuint shader, const std::string& type) {
    GLint success;
    GLchar infoLog[1024];
    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n"
                      << infoLog << "\n -- --------------------------------------------------- -- "
                      << std::endl;
        }
    } else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n"
                      << infoLog << "\n -- --------------------------------------------------- -- "
                      << std::endl;
        }
    }
}
