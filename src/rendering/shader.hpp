#ifndef SHADER_HPP
#define SHADER_HPP

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>
#include <variant>

class Shader {
public:
    // ===== Constructors =====
    Shader(const std::string& vertexPath, const std::string& fragmentPath);
    Shader(GLuint programId) : programId(programId) {}
    ~Shader();

    // ===== Usage =====
    void Use() const { glUseProgram(programId); }

    // ===== Uniform Setters =====
    void SetMat4(const std::string& name, const glm::mat4& mat) const;
    void SetMat3(const std::string& name, const glm::mat3& mat) const;
    void SetMat2(const std::string& name, const glm::mat2& mat) const;

    void SetVec4(const std::string& name, const glm::vec4& vec) const;
    void SetVec3(const std::string& name, const glm::vec3& vec) const;
    void SetVec2(const std::string& name, const glm::vec2& vec) const;

    void SetIVec4(const std::string& name, const glm::ivec4& vec) const;
    void SetIVec3(const std::string& name, const glm::ivec3& vec) const;
    void SetIVec2(const std::string& name, const glm::ivec2& vec) const;

    void SetFloat(const std::string& name, float value) const;
    void SetInt(const std::string& name, int value) const;
    void SetUInt(const std::string& name, unsigned int value) const;
    void SetBool(const std::string& name, bool value) const;

    void SetMat4Array(const std::string& name, const glm::mat4* matrices, int count) const;
    void SetVec3Array(const std::string& name, const glm::vec3* vectors, int count) const;
    void SetFloatArray(const std::string& name, const float* values, int count) const;
    void SetIntArray(const std::string& name, const int* values, int count) const;

    void SetSampler2D(const std::string& name, int textureUnit) const;
    void SetSamplerCube(const std::string& name, int textureUnit) const;

    // ===== Utility =====
    void Reload(const std::string& vertexPath, const std::string& fragmentPath);

    static std::string LoadFile(const std::string& path);
    static void CheckCompileErrors(GLuint shader, const std::string& type);

    // ===== Uniform Inspector =====
    using UniformValue = std::variant<int, float, bool, glm::vec2, glm::vec3, glm::vec4, glm::mat4, std::string>;
    std::unordered_map<std::string, UniformValue> GetActiveUniformValues() const;

    // ===== Getters & Setters =====
    GLuint GetProgramId() const { return programId; }
    void SetProgramId(GLuint id) { programId = id; }

private:
    GLuint programId;
};

#endif // SHADER_HPP
