#include "Mesh.hpp"
#include <iostream>
#include <unordered_map>

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices)
    : vertices_(vertices), indices_(indices), vao_(0), vbo_(0), ebo_(0), initialized_(false) {
    std::cout << "[Mesh] Creating mesh with " << vertices.size()
              << " vertices and " << indices.size() << " indices" << std::endl;
    SetupMesh();
}

Mesh::Mesh() noexcept
    : vao_(0), vbo_(0), ebo_(0), initialized_(false) {}

Mesh::~Mesh() {
    std::cout << "[Mesh] Destroying mesh (VAO: " << vao_
              << ", VBO: " << vbo_ << ", EBO: " << ebo_ << ")" << std::endl;
    Cleanup();
}

void Mesh::SetupMesh() {
    std::cout << "[Mesh] Setting up mesh buffers..." << std::endl;

    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);
    glGenBuffers(1, &ebo_);

    std::cout << "[Mesh] Generated buffers - VAO: " << vao_
              << ", VBO: " << vbo_ << ", EBO: " << ebo_ << std::endl;

    glBindVertexArray(vao_);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(Vertex),
                 vertices_.data(), GL_STATIC_DRAW);
    std::cout << "[Mesh] VBO allocated: " << (vertices_.size() * sizeof(Vertex)) << " bytes" << std::endl;

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() * sizeof(unsigned int),
                 indices_.data(), GL_STATIC_DRAW);
    std::cout << "[Mesh] EBO allocated: " << (indices_.size() * sizeof(unsigned int)) << " bytes" << std::endl;

    // Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void*)offsetof(Vertex, Position));

    // Normal
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void*)offsetof(Vertex, Normal));

    // Texture Coordinates
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void*)offsetof(Vertex, TexCoords));

    std::cout << "[Mesh] Vertex attributes configured" << std::endl;

    glBindVertexArray(0);
    initialized_ = true;

    std::cout << "[Mesh] Mesh setup completed successfully" << std::endl;
}

std::shared_ptr<Mesh> Mesh::LoadMesh(const std::string& name) {
    static std::unordered_map<std::string, std::shared_ptr<Mesh>> cache;

    if (cache.find(name) == cache.end()) {
        cache[name] = std::make_shared<Mesh>(MeshLoader::LoadOBJ(name));
    }

    return cache[name];
}

void Mesh::Draw(const Shader& program) const {
    if (!initialized_) {
        std::cout << "[Mesh][WARN] Attempted to draw an uninitialized mesh!" << std::endl;
        return;
    }

    program.Use();
    glBindVertexArray(vao_);

    GLsizei indexCount = static_cast<GLsizei>(indices_.size());

    if (indexCount <= 0) {
        std::cout << "[Mesh][WARN] Invalid index count: " << indexCount << std::endl;
        glBindVertexArray(0);
        return;
    }

    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void Mesh::Cleanup() noexcept {
    if (initialized_) {
        std::cout << "[Mesh] Cleaning up OpenGL resources (VAO: " << vao_
                  << ", VBO: " << vbo_ << ", EBO: " << ebo_ << ")" << std::endl;

        glDeleteVertexArrays(1, &vao_);
        glDeleteBuffers(1, &vbo_);
        glDeleteBuffers(1, &ebo_);

        vao_ = vbo_ = ebo_ = 0;
        initialized_ = false;

        std::cout << "[Mesh] Cleanup completed" << std::endl;
    }
}

Mesh::Mesh(Mesh&& other) noexcept
    : vertices_(std::move(other.vertices_)),
      indices_(std::move(other.indices_)),
      vao_(other.vao_),
      vbo_(other.vbo_),
      ebo_(other.ebo_),
      initialized_(other.initialized_) {
    std::cout << "[Mesh] Transferring resources from another mesh (VAO: " << other.vao_ << ")" << std::endl;

    other.vao_ = 0;
    other.vbo_ = 0;
    other.ebo_ = 0;
    other.initialized_ = false;
}

Mesh& Mesh::operator=(Mesh&& other) noexcept {
    if (this != &other) {
        std::cout << "[Mesh] Cleaning up current mesh and transferring from another (VAO: " << other.vao_ << ")" << std::endl;

        Cleanup();

        vertices_ = std::move(other.vertices_);
        indices_ = std::move(other.indices_);
        vao_ = other.vao_;
        vbo_ = other.vbo_;
        ebo_ = other.ebo_;
        initialized_ = other.initialized_;

        other.vao_ = 0;
        other.vbo_ = 0;
        other.ebo_ = 0;
        other.initialized_ = false;
    }
    return *this;
}
