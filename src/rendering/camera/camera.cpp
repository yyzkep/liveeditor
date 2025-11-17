#include "Camera.hpp"
#include <iostream>

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
    : front_(glm::vec3(0.0f, 0.0f, -3.0f)),
      speed_(2.5f),
      sensitivity_(0.1f),
      zoom_(90.0f),
      position_(position),
      worldUp_(up),
      yaw_(yaw),
      pitch_(pitch) {
    
    std::cout << "[CAMERA] Creating camera at position (" 
              << position.x << ", " << position.y << ", " << position.z 
              << "), yaw: " << yaw << ", pitch: " << pitch << std::endl;

    UpdateVectors();
}

void Camera::Update(GLFWwindow* window, float deltaTime) {
    bool moved = false;
    bool rotated = false;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        ProcessKeyboard(CameraMovement::Forward, deltaTime);
        moved = true;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        ProcessKeyboard(CameraMovement::Backward, deltaTime);
        moved = true;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        ProcessKeyboard(CameraMovement::Left, deltaTime);
        moved = true;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        ProcessKeyboard(CameraMovement::Right, deltaTime);
        moved = true;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        ProcessKeyboard(CameraMovement::Up, deltaTime);
        moved = true;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        ProcessKeyboard(CameraMovement::Down, deltaTime);
        moved = true;
    }

    static bool firstMouse = true;
    static double lastX = 0.0, lastY = 0.0;

    double currentX, currentY;
    glfwGetCursorPos(window, &currentX, &currentY);

    if (firstMouse) {
        lastX = currentX;
        lastY = currentY;
        firstMouse = false;
        std::cout << "[CAMERA] Mouse input initialized" << std::endl;
    }

    float xOffset = static_cast<float>(currentX - lastX);
    float yOffset = static_cast<float>(lastY - currentY);
    lastX = currentX;
    lastY = currentY;

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        if (std::abs(xOffset) > 0.1f || std::abs(yOffset) > 0.1f) {
            ProcessMouse(xOffset, yOffset);
            rotated = true;
        }
    } else {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}

glm::mat4 Camera::GetViewMatrix() const {
    return glm::lookAt(position_, position_ + front_, up_);
}

glm::mat4 Camera::GetProjectionMatrix(float aspectRatio, float fovDeg, float nearPlane, float farPlane) const {
    return glm::perspective(glm::radians(fovDeg), aspectRatio, nearPlane, farPlane);
}

void Camera::ProcessKeyboard(CameraMovement direction, float deltaTime) {
    float velocity = speed_ * deltaTime;

    if (direction == CameraMovement::Forward)
        position_ += front_ * velocity;
    if (direction == CameraMovement::Backward)
        position_ -= front_ * velocity;
    if (direction == CameraMovement::Left)
        position_ -= right_ * velocity;
    if (direction == CameraMovement::Right)
        position_ += right_ * velocity;
    if (direction == CameraMovement::Up)
        position_ += worldUp_ * velocity;
    if (direction == CameraMovement::Down)
        position_ -= worldUp_ * velocity;
}

void Camera::ProcessMouse(float xOffset, float yOffset, bool constrainPitch) {
    xOffset *= sensitivity_;
    yOffset *= sensitivity_;

    yaw_ += xOffset;
    pitch_ += yOffset;

    if (constrainPitch) {
        if (pitch_ > 89.0f)
            pitch_ = 89.0f;
        if (pitch_ < -89.0f)
            pitch_ = -89.0f;
    }

    UpdateVectors();
}

void Camera::ProcessScroll(float yOffset) {
    float oldZoom = zoom_;
    zoom_ -= yOffset;
    
    if (zoom_ < 1.0f)
        zoom_ = 1.0f;
    if (zoom_ > 90.0f)
        zoom_ = 90.0f;

    if (std::abs(oldZoom - zoom_) > 0.5f) {
        std::cout << "[CAMERA] Zoom changed from " << oldZoom << " to " << zoom_ << std::endl;
    }
}

void Camera::UpdateVectors() {
    glm::vec3 front;
    front.x = cos(glm::radians(yaw_)) * cos(glm::radians(pitch_));
    front.y = sin(glm::radians(pitch_));
    front.z = sin(glm::radians(yaw_)) * cos(glm::radians(pitch_));
    front_ = glm::normalize(front);
    right_ = glm::normalize(glm::cross(front_, worldUp_));
    up_ = glm::normalize(glm::cross(right_, front_));
}
