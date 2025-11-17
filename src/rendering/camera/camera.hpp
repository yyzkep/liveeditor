#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum class CameraMovement { Forward, Backward, Left, Right, Up, Down };

class Camera {
public:
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f),
           glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
           float yaw = -90.0f, float pitch = 0.0f);

    glm::mat4 GetViewMatrix() const;
    glm::mat4 GetProjectionMatrix(float aspectRatio,
                                  float fovDeg = 45.0f,
                                  float nearPlane = 0.1f,
                                  float farPlane = 100.0f) const;

    void ProcessKeyboard(CameraMovement direction, float deltaTime);
    void ProcessMouse(float xOffset, float yOffset, bool constrainPitch = true);
    void ProcessScroll(float yOffset);
    void Update(GLFWwindow* window, float deltaTime);

    // Getters
    glm::vec3 GetPosition() const { return position_; }
    glm::vec3 GetFront() const { return front_; }
    glm::vec3 GetUp() const { return up_; }
    glm::vec3 GetRight() const { return right_; }
    glm::vec3 GetWorldUp() const { return worldUp_; }

    float GetYaw() const { return yaw_; }
    float GetPitch() const { return pitch_; }
    float GetSpeed() const { return speed_; }
    float GetSensitivity() const { return sensitivity_; }
    float GetZoom() const { return zoom_; }

    // Setters
    void SetPosition(const glm::vec3& position) { position_ = position; UpdateVectors(); }
    void SetFront(const glm::vec3& front) { front_ = front; UpdateVectors(); }
    void SetUp(const glm::vec3& up) { up_ = up; UpdateVectors(); }
    void SetRight(const glm::vec3& right) { right_ = right; UpdateVectors(); }
    void SetWorldUp(const glm::vec3& worldUp) { worldUp_ = worldUp; UpdateVectors(); }

    void SetYaw(float yaw) { yaw_ = yaw; UpdateVectors(); }
    void SetPitch(float pitch) { pitch_ = pitch; UpdateVectors(); }
    void SetSpeed(float speed) { speed_ = speed; }
    void SetSensitivity(float sensitivity) { sensitivity_ = sensitivity; }
    void SetZoom(float zoom) { zoom_ = zoom; }

private:
    void UpdateVectors();

private:
    glm::vec3 position_;
    glm::vec3 front_;
    glm::vec3 up_;
    glm::vec3 right_;
    glm::vec3 worldUp_;

    float yaw_;
    float pitch_;
    float speed_;
    float sensitivity_;
    float zoom_;
};

#endif // CAMERA_HPP
