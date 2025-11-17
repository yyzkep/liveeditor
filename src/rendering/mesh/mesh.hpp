#ifndef MESH_HPP
#define MESH_HPP

#include "../shader.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <vector>

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

class Mesh {
public:
    Mesh() noexcept;
    Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
    ~Mesh();

    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    Mesh(Mesh&& other) noexcept;
    Mesh& operator=(Mesh&& other) noexcept;

    void Draw(const Shader& program) const;

    static std::shared_ptr<Mesh> LoadMesh(const std::string& name);

private:
    void SetupMesh();
    void Cleanup() noexcept;

private:
    std::vector<Vertex> vertices_;
    std::vector<unsigned int> indices_;

    GLuint vao_{0};
    GLuint vbo_{0};
    GLuint ebo_{0};
    bool initialized_{false};
};

namespace MeshLoader {
    Mesh LoadOBJ(const std::string& path);
}

#endif // MESH_HPP
