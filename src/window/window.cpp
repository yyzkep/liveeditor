#include "Window.hpp"
#include <iostream>

Window::Window(int width, int height, const char* title, int versionMajor, int versionMinor, int profile) {
    std::cout << "[Window] Creating window: " << width << "x" << height << " - " << title << std::endl;
    Initialize(versionMajor, versionMinor, profile);
    CreateWindow(width, height, title);
}

Window::Window(int width, int height, const char* title, int msaaSamples,
               int versionMajor, int versionMinor, int profile) {
    std::cout << "[Window] Creating window: " << width << "x" << height
              << " - " << title << " (MSAA: " << msaaSamples << "x)" << std::endl;

    this->msaaSamples = msaaSamples;
    Initialize(versionMajor, versionMinor, profile);
    CreateWindow(width, height, title);
}

Window::~Window() {
    Shutdown();
}

void Window::Initialize(int major, int minor, int profile) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor);
    glfwWindowHint(GLFW_OPENGL_PROFILE, profile);

    glfwWindowHint(GLFW_SAMPLES, msaaSamples);
}

void Window::CreateWindow(int width, int height, const char* title) {
    window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (window == nullptr) {
        std::cout << "[Window][Error] Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "[Window][Error] Failed to initialize GLAD" << std::endl;
        return;
    }

    screenWidth = width;
    screenHeight = height;

    glViewport(0, 0, screenWidth, screenHeight);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);

    GLint samples;
    glGetIntegerv(GL_SAMPLES, &samples);
    std::cout << "[Window] MSAA samples: " << samples << "x" << std::endl;

    std::cout << "[Window] Window created successfully: " << width << "x" << height
              << " (MSAA: " << samples << "x)" << std::endl;

    ImGuiInitialize();
}

void Window::BeginFrame() {
    ProcessInput();
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    ImGuiBeginFrame();
}

void Window::ProcessInput() {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void Window::ImGuiInitialize() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    std::cout << "[Window] ImGui initialized" << std::endl;
}

void Window::ImGuiBeginFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void Window::ImGuiRender() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Window::ImGuiShutdown() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    std::cout << "[Window] ImGui shutdown" << std::endl;
}

void Window::EndFrame() {
    ImGuiRender();
    glfwPollEvents();
    glfwSwapBuffers(window);
}

void Window::Shutdown() {
    std::cout << "[Window] Shutting down" << std::endl;
    ImGuiShutdown();
    glfwTerminate();
}

void Window::CheckAntiAliasingStatus() const {
    GLint samples;
    glGetIntegerv(GL_SAMPLES, &samples);

    GLboolean msaaEnabled = glIsEnabled(GL_MULTISAMPLE);

    std::cout << "[Window] Anti-aliasing status:" << std::endl;
    std::cout << "  MSAA Enabled: " << (msaaEnabled ? "Yes" : "No") << std::endl;
    std::cout << "  Samples: " << samples << "x" << std::endl;

    GLint maxSamples;
    glGetIntegerv(GL_MAX_SAMPLES, &maxSamples);
    std::cout << "  Maximum supported samples: " << maxSamples << "x" << std::endl;
}

void Window::SetMsaaSamples(int samples) {
    if (samples != msaaSamples) {
        std::cout << "[Window] Changing MSAA samples from " << msaaSamples
                  << "x to " << samples << "x" << std::endl;
        msaaSamples = samples;
    }
}

void Window::GetFramebufferSize(int& width, int& height) const {
    glfwGetFramebufferSize(window, &width, &height);
}

void Window::ToggleAntiAliasing(bool enable) {
    if (enable) {
        glEnable(GL_MULTISAMPLE);
        std::cout << "[Window] Anti-aliasing enabled" << std::endl;
    } else {
        glDisable(GL_MULTISAMPLE);
        std::cout << "[Window] Anti-aliasing disabled" << std::endl;
    }
}
