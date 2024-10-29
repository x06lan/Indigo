#include "pch.hpp"

#include <map>

#include "log.hpp"
#include "exception.hpp"
#include "window.hpp"
#include "renderer.hpp"
#include "pipeline.hpp"
#include "spirv.hpp"
#include "controller.hpp"
#include "camera.hpp"
#include "importer.hpp"
#include "vertex_array.hpp"
#include "texture.hpp"
#include "transform.hpp"
#include "scene.hpp"
#include "model.hpp"
#include "light.hpp"

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

int main(int argc, char **argv) {
    Log::Init();

    if (argc > 1) {
        if (strncmp(argv[1], "-d", 2) == 0)
            Log::SetLevel(Log::DEBUG);

        if (strncmp(argv[1], "-t", 2) == 0)
            Log::SetLevel(Log::TRACE);
    }

    Window window(SCREEN_WIDTH, SCREEN_HEIGHT);

    Renderer::Init();
    Renderer::ClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    Controller::InitGUI(window);
    std::string vertPath = "../assets/shaders/test.vert.spv";
    std::string fragPath = "../assets/shaders/test.frag.spv";
    if (argc > 3) {
        vertPath = argv[2];
        fragPath = argv[3];
    }

    ScreenRenderer screenRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, vertPath,
                                  fragPath);

    do {

        screenRenderer.Render();

        Renderer::DisableDepthTest();

        // glfwSwapInterval(0);
        glfwSwapBuffers(window.GetWindow());
        window.PollEvents();
    } while (!window.ShouldClose());
    // pipeline.SavePass(Pipeline::LIGHTING, "lighting.png");

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
