#include "window/window.hpp"
#include "logging/logger.hpp"
#include "world/world.hpp"

int main() {
    CLogger logger;
    logger.Info("program start");
    
    try
    {
        Window window{800, 600, "etherion"};
        Camera camera;
        World world;
        while(!glfwWindowShouldClose(window.GetGlfwWindow())) {
            window.ProcessInput();
            window.BeginFrame();

            window.EndFrame();
        }
    }
    catch(const std::exception& e)
    {
        logger.Error(e.what());
    }
    
    logger.Info("program end");
    return 0;
}