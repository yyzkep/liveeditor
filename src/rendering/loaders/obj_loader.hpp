#ifndef OBJ_LOADER_HPP
#define OBJ_LOADER_HPP

#include "../mesh/Mesh.hpp"
#include <algorithm>
#include <fstream>
#include <glm/glm.hpp>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

namespace MeshLoader {

inline Mesh LoadOBJ(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "[ObjLoader] Failed to open: " << path << std::endl;
        return {};
    }

    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texCoords;
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#')
            continue;

        std::stringstream ss(line);
        std::string prefix;
        ss >> prefix;

        if (prefix == "v") {
            glm::vec3 v;
            ss >> v.x >> v.y >> v.z;
            positions.push_back(v);
        } 
        else if (prefix == "vn") {
            glm::vec3 n;
            ss >> n.x >> n.y >> n.z;
            normals.push_back(n);
        } 
        else if (prefix == "vt") {
            glm::vec2 uv;
            ss >> uv.x >> uv.y;
            texCoords.push_back(uv);
        } 
        else if (prefix == "f") {
            std::vector<unsigned int> faceIndices;
            std::string vertexDef;

            while (ss >> vertexDef) {
                std::replace(vertexDef.begin(), vertexDef.end(), '/', ' ');
                std::stringstream vss(vertexDef);

                int vi = 0, ti = 0, ni = 0;
                vss >> vi;
                if (!vss.eof()) vss >> ti;
                if (!vss.eof()) vss >> ni;

                if (vi <= 0 || vi > static_cast<int>(positions.size())) {
                    std::cerr << "[ObjLoader][WARN] Invalid vertex index in " << path << std::endl;
                    continue;
                }

                Vertex vert{};
                vert.Position = positions[vi - 1];
                if (ti > 0 && ti <= static_cast<int>(texCoords.size()))
                    vert.TexCoords = texCoords[ti - 1];
                if (ni > 0 && ni <= static_cast<int>(normals.size()))
                    vert.Normal = normals[ni - 1];

                vertices.push_back(vert);
                faceIndices.push_back(static_cast<unsigned int>(vertices.size() - 1));
            }

            // Triangulate polygonal faces
            for (size_t i = 1; i + 1 < faceIndices.size(); ++i) {
                indices.push_back(faceIndices[0]);
                indices.push_back(faceIndices[i]);
                indices.push_back(faceIndices[i + 1]);
            }
        }
    }

    std::cout << "[ObjLoader] Loaded " << vertices.size()
              << " vertices and " << indices.size()
              << " indices from " << path << std::endl;

    return Mesh(vertices, indices);
}

} // namespace MeshLoader

#endif // OBJ_LOADER_HPP
