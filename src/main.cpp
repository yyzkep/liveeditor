#include "window/window.hpp"
#include "logging/logger.hpp"
#include "world/world.hpp"
#include "console/console.hpp"

#include <thread>
#include <Windows.h>

int main()
{
    CLogger logger;
    logger.Info("program start");

    try
    {
        Window window{800, 600, "etherion"};
        Camera camera{};
        World world{"main_world"};
        world.SetCamera(camera);
        ImGuiIO *io = &ImGui::GetIO();
        Console console{&world, io};
        while (!glfwWindowShouldClose(window.GetGlfwWindow()))
        {
            window.ProcessInput();
            window.BeginFrame();

            camera.Update(window.GetGlfwWindow(), window.GetDeltaTime());
            console.Update(window.GetDeltaTime());
            if (console.WantsInput())
            {
             //TODO: FIX CONSOLE FUCK   
            }

            console.Draw();
            world.DrawAll(static_cast<float>(window.GetScreenWidth() / window.GetScreenHeight()));
            window.EndFrame();
        }
    }
    catch (const std::exception &e)
    {
        logger.Error(e.what());
        Sleep(10000);
        return -1;
    }

    logger.Info("program end");
    return 0;
}