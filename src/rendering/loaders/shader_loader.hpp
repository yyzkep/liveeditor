#ifndef SHADER_LOADER_HPP
#define SHADER_LOADER_HPP

#include "../shader.hpp"
#include <unordered_map>
#include <memory>
#include <string>
#include <iostream>

namespace ShaderLoader {

inline std::shared_ptr<Shader> LoadShader(const std::string& vertPath, const std::string& fragPath) {
    static std::unordered_map<std::string, std::shared_ptr<Shader>> cache;
    std::string key = vertPath + "|" + fragPath;

    if (auto it = cache.find(key); it != cache.end())
        return it->second;

    auto shaderProgram = std::make_shared<Shader>(vertPath.c_str(), fragPath.c_str());
    cache[key] = shaderProgram;

    std::cout << "[ShaderLoader] Loaded shader pair: "
              << vertPath << " + " << fragPath << std::endl;

    return shaderProgram;
}

} // namespace ShaderLoader

#endif // SHADER_LOADER_HPP
