#ifndef PARAMS
#define PARAMS
#ifndef PARAMS_HPP
#define PARAMS_HPP

#include <glm/glm.hpp>
#include <iostream>
#include <string>
#include <unordered_map>
#include <variant>
#include "../rendering/Shader.hpp" // For Shader::UniformValue_t

class CParams {
public:
    using Value = Shader::UniformValue;

    // --- Generic Setters ---
    void Set(const std::string& name, Value value) {
        values_[name] = std::move(value);
    }

    template<typename T>
    void Set(const std::string& name, const T& value) {
        values_[name] = value;
    }

    // --- Generic Getters ---
    Value* Get(const std::string& name) {
        auto it = values_.find(name);
        if (it != values_.end())
            return &it->second;
        return nullptr;
    }

    const Value* Get(const std::string& name) const {
        auto it = values_.find(name);
        if (it != values_.end())
            return &it->second;
        return nullptr;
    }

    template<typename T>
    T Get(const std::string& name) const {
        auto it = values_.find(name);
        if (it == values_.end())
            throw std::runtime_error("[Params] Attempted to get nonexistent key: " + name);

        if (auto val = std::get_if<T>(&it->second))
            return *val;

        throw std::runtime_error("[Params] Type mismatch for key: " + name);
    }

    template<typename T>
    T GetOr(const std::string& name, const T& defaultValue) const {
        auto it = values_.find(name);
        if (it != values_.end()) {
            if (auto val = std::get_if<T>(&it->second))
                return *val;
        }
        return defaultValue;
    }

    // --- Management ---
    bool Remove(const std::string& name) {
        return values_.erase(name) > 0;
    }

    bool Has(const std::string& name) const {
        return values_.find(name) != values_.end();
    }

    const auto& All() const { return values_; }

private:
    std::unordered_map<std::string, Value> values_;
};

#endif // PARAMS_HPP


#endif /* PARAMS */
