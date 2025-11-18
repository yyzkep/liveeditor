#include "console/console.hpp"
#include "logging/logger.hpp"
#include "window/window.hpp"
#include "world/world.hpp"

#include <Windows.h>
#include <thread>

int main() {
    CLogger logger;
    logger.Info("program start");

    try {
        Window window{800, 600, "etherion"};
        Camera camera{};
        World world{"main_world"};
        world.SetCamera(camera);
        ImGuiIO *io = &ImGui::GetIO();
        Console console{&world, io};
        while (!glfwWindowShouldClose(window.GetGLFWwindow())) {
            window.ProcessInput();
            window.BeginFrame();
            camera.Update(window.GetGLFWwindow(), window.GetDeltaTime());

            if (console.WantsInput()) {
                console.Update(window.GetDeltaTime());
            }

            console.Draw();

            world.DrawAll(static_cast<float>(window.GetScreenWidth() / window.GetScreenHeight()));
            window.EndFrame();
        }
    } catch (const std::exception &e) {
        logger.Error(e.what());
        Sleep(10000);
        return -1;
    }

    logger.Info("program end");
    return 0;
}