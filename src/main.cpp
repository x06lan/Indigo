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

    // Pipeline pipeline(SCREEN_WIDTH, SCREEN_HEIGHT);
    ScreenRenderer screenRenderer(SCREEN_WIDTH, SCREEN_HEIGHT);

    // pipeline.Init();

    std::shared_ptr<Camera> mainCamera =
        std::make_shared<Camera>(glm::vec3{350, 200, 100}, 45.0f,
                                 window.GetAspectRatio(), 10.0f, 5000.0f);
    // mainCamera->GetTransform().SetPosition({270, 200, 100});
    Scene scene(mainCamera);

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
