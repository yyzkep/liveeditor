#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

class Window {
public:
    // Constructors
    Window(int width, int height, const char* title,
           int versionMajor = 3, int versionMinor = 3, int profile = GLFW_OPENGL_CORE_PROFILE);

    Window(int width, int height, const char* title, int msaaSamples,
           int versionMajor = 3, int versionMinor = 3, int profile = GLFW_OPENGL_CORE_PROFILE);

    // Destructor
    ~Window();

    // Initialization and main loop
    void Initialize(int major, int minor, int profile);
    void CreateWindow(int width, int height, const char* title);
    void BeginFrame();
    void ProcessInput();
    void EndFrame();
    void Shutdown();

    // ImGui integration
    void ImGuiInitialize();
    void ImGuiBeginFrame();
    void ImGuiRender();
    void ImGuiShutdown();

    // Anti-aliasing and framebuffer
    void CheckAntiAliasingStatus() const;
    void SetMsaaSamples(int samples);
    void ToggleAntiAliasing(bool enable);
    void GetFramebufferSize(int& width, int& height) const;

    // Time
    float GetDeltaTime() {
        float currentFrame = static_cast<float>(glfwGetTime());
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        return deltaTime;
    }

    // ======= Getters & Setters =======
    GLFWwindow* GetGlfwWindow() const { return window; }

    int GetScreenWidth() const { return screenWidth; }
    void SetScreenWidth(int width) { screenWidth = width; }

    int GetScreenHeight() const { return screenHeight; }
    void SetScreenHeight(int height) { screenHeight = height; }

    int GetMsaaSamples() const { return msaaSamples; }

private:
    GLFWwindow* window = nullptr;
    int screenWidth = 0;
    int screenHeight = 0;
    int msaaSamples = 4;

    float lastFrame = 0.0f;
};

#endif /* WINDOW_HPP */
