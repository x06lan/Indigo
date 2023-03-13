#include "pch.hpp"

#include "log.hpp"
#include "exception.hpp"
#include "window.hpp"
#include "renderer.hpp"
#include "controller.hpp"
#include "program.hpp"
#include "camera.hpp"
#include "importer.hpp"
#include "vertex_array.hpp"
#include "vertex_buffer.hpp"
#include "index_buffer.hpp"
#include "frame_buffer.hpp"
#include "uniform_buffer.hpp"
#include "texture.hpp"
#include "transform.hpp"
#include "model.hpp"
#include "light.hpp"

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

#define SHADOW_SIZE 1024 * 2

#define LIGHT_NUMBER 2

#pragma pack(16) // std140 layout pads by multiple of 16
struct Matrices {
    glm::mat4 model;
    glm::mat4 viewProjection;
};

struct Material {
    glm::vec3 baseColor;
    float maxShine;
};

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
    Renderer::ClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    Controller::InitGUI(window);

    enum {
        ALBEDO,
        NORMAL,
        ARM,
        EMISSION,
        REFLECT,
        POSITION,
        DEPTH,
        LIGHTING,
        VOLUME,
        SHADOW
    };
    Program programShadow("../assets/shaders/shadow.vert",
                          "../assets/shaders/shadow.geom",
                          "../assets/shaders/shadow.frag");

    Program programDeferredPass("../assets/shaders/phong.vert",
                                "../assets/shaders/deferred_pass.frag");
    programDeferredPass.Bind();
    programDeferredPass.SetInt("albedoMap", ALBEDO);
    programDeferredPass.SetInt("normalMap", NORMAL);
    programDeferredPass.SetInt("emissionMap", EMISSION);
    programDeferredPass.SetInt("reflectMap", REFLECT);
    programDeferredPass.SetInt("ARM", ARM);

    Program programDeferredLight("../assets/shaders/frame_deferred.vert",
                                 "../assets/shaders/deferred_light.frag");
    programDeferredLight.Bind();
    programDeferredLight.SetInt("screenAlbedo", ALBEDO);
    programDeferredLight.SetInt("screenNormal", NORMAL);
    programDeferredLight.SetInt("screenPosition", POSITION);
    programDeferredLight.SetInt("screenEmission", EMISSION);
    programDeferredLight.SetInt("reflectMap", REFLECT);
    programDeferredLight.SetInt("screenARM", ARM);
    programDeferredLight.SetInt("screenDepth", DEPTH);

    for (int i = 0; i < LIGHT_NUMBER; i++) {
        programDeferredPass.Bind();
        programDeferredPass.SetInt("shadowMap[" + std::to_string(i) + "]",
                                   SHADOW + i);
        programDeferredLight.Bind();
        programDeferredLight.SetInt("shadowMap[" + std::to_string(i) + "]",
                                    SHADOW + i);
    }

    Program programScreen("../assets/shaders/frame_screen.vert",
                          "../assets/shaders/frame_screen.frag");
    programScreen.Bind();
    programScreen.SetInt("screenAlbedo", ALBEDO);
    programScreen.SetInt("screenNormal", NORMAL);
    programScreen.SetInt("screenPosition", POSITION);
    programScreen.SetInt("screenEmission", EMISSION);
    programScreen.SetInt("reflectMap", REFLECT);
    programScreen.SetInt("screenARM", ARM);
    programScreen.SetInt("screenLight", LIGHTING);
    programScreen.SetInt("screenVolume", VOLUME);
    programScreen.SetInt("screenDepth", DEPTH);

    // Small hack to put camera position into the shader
    // TODO: Find somewhere on the UBO to put this in
    // GLint cameraUniform =
    //     glGetUniformLocation(programColor.GetProgramID(), "cameraPosition");
    GLint cameraUniformDeferredPass = glGetUniformLocation(
        programDeferredPass.GetProgramID(), "cameraPosition");
    GLint cameraUniformDeferredLight = glGetUniformLocation(
        programDeferredLight.GetProgramID(), "cameraPosition");

    LightData lightInfo[LIGHT_NUMBER];

    UniformBuffer matrices(sizeof(Matrices), 0);
    UniformBuffer materials(sizeof(Material), 1);
    UniformBuffer lights(sizeof(LightData) * LIGHT_NUMBER, 2);
    UniformBuffer cameraUbo(sizeof(CameraData), 3);

    Camera camera(45.0f, window.GetAspectRatio());

    Light light1(Light::POINT, glm::vec3(1.0f));
    Light light2(Light::POINT, glm::vec3(1.0f));

    std::vector<Model> scene;

    // begin model 1
    Material matColor1 = {glm::vec3(0.8f, 0.5f, 0.0f), 30.0f};

    std::vector<Controller::TransformSlider> transformSliders;
    std::vector<Controller::LightSlider> lightSliders;

    transformSliders.push_back(Controller::TransformSlider("Model 1",       //
                                                           {0, 0, 0},       //
                                                           {180, 180, 180}, //
                                                           {1, 1, 1}));
    try {
        scene.push_back(
            Model(Importer::LoadFile("../assets/models/little_city/main.glb")));
    } catch (std::exception &e) {
        LOG_ERROR("{}", e.what());
    }
    // end model 1

    // begin model 2
    // Material matColor2 = {{0.0f, 0.8f, 0.8f}, 100.0f};

    transformSliders.push_back(Controller::TransformSlider("Model 2",       //
                                                           {0, 0, 0},       //
                                                           {180, 180, 180}, //
                                                           {1, 1, 1}));
    try {
        scene.push_back(Model(
            Importer::LoadFile("../assets/models/little_city/interior.glb")));
    } catch (std::exception &e) {
        LOG_ERROR("{}", e.what());
    }
    // end model 2

    // 2D plane for framebuffer
    VertexArray planeVAO;

    // vertices
    planeVAO.AddVertexBuffer(std::make_shared<VertexBuffer>(
        std::vector<float>{
            -1.0f, 1.0f,  //
            -1.0f, -1.0f, //
            1.0f, -1.0f,  //
            1.0f, 1.0f,   //
        },
        2 * sizeof(float)));

    // UV
    planeVAO.AddVertexBuffer(std::make_shared<VertexBuffer>(
        std::vector<float>{
            0.0f, 1.0f, //
            0.0f, 0.0f, //
            1.0f, 0.0f, //
            1.0f, 1.0f, //
        },
        2 * sizeof(float)));

    // Indices
    planeVAO.SetIndexBuffer(
        std::make_shared<IndexBuffer>(std::vector<unsigned int>{
            0, 1, 2, //
            0, 2, 3, //
        }));

    Model plane(std::make_shared<VertexArray>(planeVAO));

    Texture texMainColor("../assets/textures/little_city/main_color.jpg");
    Texture texInterior("../assets/textures/little_city/interior.jpg");
    Texture reflectMap("../assets/textures/vestibule_2k.hdr");
    Texture wallNormalMap("../assets/textures/T_Wall_Damaged_2x1_A_N.png");
    Texture wallAOMap("../assets/textures/T_Wall_Damaged_2x1_A_AO.png");

    FrameBuffer deferredFbo;
    deferredFbo.Bind();
    unsigned int attachments[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1,
                                  GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3,
                                  GL_COLOR_ATTACHMENT4};

#pragma region color buffer
    Texture screenAlbedo(SCREEN_WIDTH, SCREEN_HEIGHT, Texture::RGBA);
    deferredFbo.AttachTexture(screenAlbedo.GetTextureID(), attachments[0]);
    Texture screenNormal(SCREEN_WIDTH, SCREEN_HEIGHT, Texture::RGBA);
    deferredFbo.AttachTexture(screenNormal.GetTextureID(), attachments[1]);
    Texture screenPosition(SCREEN_WIDTH, SCREEN_HEIGHT, Texture::RGBA);
    deferredFbo.AttachTexture(screenPosition.GetTextureID(), attachments[2]);
    Texture screenARM(SCREEN_WIDTH, SCREEN_HEIGHT, Texture::RGBA);
    deferredFbo.AttachTexture(screenARM.GetTextureID(), attachments[3]);
    Texture screenEmission(SCREEN_WIDTH, SCREEN_HEIGHT, Texture::RGBA);
    deferredFbo.AttachTexture(screenEmission.GetTextureID(), attachments[4]);
    Texture screenDepth(SCREEN_WIDTH, SCREEN_HEIGHT, Texture::DEPTH);
    deferredFbo.AttachTexture(screenDepth.GetTextureID(), GL_DEPTH_ATTACHMENT);
    glDrawBuffers(5, attachments);
    deferredFbo.Unbind();
#pragma endregion

#pragma region lighting pass texture
    FrameBuffer deferredLightFbo;
    deferredLightFbo.Bind();
    Texture screenLight(SCREEN_WIDTH, SCREEN_HEIGHT, Texture::RGBA);
    deferredLightFbo.AttachTexture(screenLight.GetTextureID(), attachments[0]);
    Texture screenVolume(SCREEN_WIDTH, SCREEN_HEIGHT, Texture::RGBA);
    deferredLightFbo.AttachTexture(screenVolume.GetTextureID(), attachments[1]);
    glDrawBuffers(2, attachments);

    GLuint rbo1;

    glCreateRenderbuffers(1, &rbo1);
    glNamedRenderbufferStorage(rbo1, GL_DEPTH24_STENCIL8, SCREEN_WIDTH,
                               SCREEN_HEIGHT);

    glNamedFramebufferRenderbuffer(deferredLightFbo.GetBufferID(),
                                   GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER,
                                   rbo1);

    deferredLightFbo.Unbind();
#pragma endregion

#pragma region shadow
    FrameBuffer shadowFbo;
    shadowFbo.Bind();
    std::vector<std::shared_ptr<Texture>> lightDepths;
    for (int i = 0; i < LIGHT_NUMBER; i++) {
        lightDepths.push_back(std::make_shared<Texture>(
            SHADOW_SIZE, SHADOW_SIZE, Texture::DEPTH, Texture::CUBE));
    }
#pragma endregion

    // light 1
    transformSliders.push_back(                     //
        Controller::TransformSlider("Light 1",      //
                                    {50, 100, 200}, //
                                    {0, 0, 0},      //
                                    {20, 20, 20}));
    lightSliders.push_back(Controller::LightSlider("Light 1", 1, 500));

    // light 2
    transformSliders.push_back(                     //
        Controller::TransformSlider("Light 2",      //
                                    {-300, 300, 0}, //
                                    {0, 0, 0},      //
                                    {20, 20, 20}));
    lightSliders.push_back(Controller::LightSlider("Light 2", 2, 500));

    scene.push_back(Model(Importer::LoadFile("../assets/models/sphere.obj")));
    scene.push_back(Model(Importer::LoadFile("../assets/models/sphere.obj")));

    Matrices lightMat;

    do {
        // TODO: Make lights into a vector
        light1.SetTransform(transformSliders[2].GetTransform());
        light1.SetPower(lightSliders[0].GetPower());
        light1.SetRadius(lightSliders[0].GetRadius());

        light2.SetTransform(transformSliders[3].GetTransform());
        light2.SetPower(lightSliders[1].GetPower());
        light2.SetRadius(lightSliders[1].GetRadius());
        lightInfo[0] = light1.GetLightData();
        lightInfo[1] = light2.GetLightData();
        glm::vec2 delta = window.GetCursorDelta();
        window.UpdateCursorPosition();

#pragma region draw shadow

        // make sure render size is same as texture
        glViewport(0, 0, SHADOW_SIZE, SHADOW_SIZE);
        Renderer::EnableDepthTest();
        Renderer::DisableCullFace();

        // every light
        programShadow.Bind();
        for (unsigned int i = 0; i < lightDepths.size(); i++) {
            shadowFbo.AttachTexture(lightDepths[i]->GetTextureID(),
                                    GL_DEPTH_ATTACHMENT);
            shadowFbo.Bind();
            Renderer::Clear();

            // not render light ball
            for (unsigned int j = 0; j < scene.size() - 2; j++) {
                scene[j].SetTransform(transformSliders[j].GetTransform());

                lightMat.model = scene[j].GetTransform().GetTransformMatrix();
                matrices.SetData(0, sizeof(Matrices), &lightMat);
                // use first one to render shadow
                lights.SetData(0, sizeof(LightData), &lightInfo[i]);
                programShadow.Validate();

                scene[j].Draw();
            }
            shadowFbo.Unbind();
        }
        programShadow.Unbind();

        Renderer::EnableCullFace();
#pragma endregion

        // color (phong shader)
        glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

        camera.GetTransform().SetPosition(
            camera.GetTransform().GetPosition() +
            10 * window.GetScrollOffset().y *
                glm::normalize(camera.GetTransform().GetPosition()));

        if (window.GetMouseButton(GLFW_MOUSE_BUTTON_RIGHT)) {
            camera.RotateByDelta(delta.x * -2 / window.GetWidth(),
                                 delta.y * -2 / window.GetHeight());
        }

        glm::vec3 cameraPos = camera.GetTransform().GetPosition();
        camera.UpdateView();

        texMainColor.Bind(ALBEDO);
        // texMainColor.Bind(EMISSION);
        reflectMap.Bind(REFLECT);
        wallAOMap.Bind(EMISSION);
// wallNormalMap.Bind(NORMAL);

// deferred
#pragma region basic pass
        Renderer::EnableDepthTest();
        deferredFbo.Bind();
        programDeferredPass.Bind();
        Renderer::Clear();
        glUniform3fv(cameraUniformDeferredPass, 1, &cameraPos.x);
        for (unsigned int i = 0; i < scene.size(); i++) {
            scene[i].SetTransform(transformSliders[i].GetTransform());

            Matrices mat1;

            mat1.model = scene[i].GetTransform().GetTransformMatrix();
            mat1.viewProjection = camera.GetViewProjection();
            matrices.SetData(0, sizeof(mat1), &mat1);
            materials.SetData(0, sizeof(Material), &matColor1);
            lights.SetData(0, sizeof(LightData) * LIGHT_NUMBER, &lightInfo);
            programDeferredPass.Validate();
            scene[i].Draw();
        }
        programDeferredPass.Unbind();
        deferredFbo.Unbind();
#pragma endregion

// deferred lighting
#pragma region deferred lighting
        Renderer::EnableDepthTest();
        deferredLightFbo.Bind();
        programDeferredLight.Bind();
        screenAlbedo.Bind(ALBEDO);
        screenNormal.Bind(NORMAL);
        screenPosition.Bind(POSITION);
        screenEmission.Bind(EMISSION);
        screenDepth.Bind(DEPTH);
        for (unsigned int i = 0; i < lightDepths.size(); i++) {
            lightDepths[i]->Bind(SHADOW + i);
        }
        // geo
        planeVAO.Bind();
        Matrices mat2;
        mat2.model = scene[0].GetTransform().GetTransformMatrix();
        mat2.viewProjection = camera.GetViewProjection();
        materials.SetData(0, sizeof(Material), &matColor1);
        lights.SetData(0, sizeof(LightData) * LIGHT_NUMBER, &lightInfo);
        CameraData camData = camera.GetCameraData();
        cameraUbo.SetData(0, sizeof(CameraData), &camData);
        // TODO: Move cameraPos to cameraUbo
        glUniform3fv(cameraUniformDeferredLight, 1, &cameraPos.x);

        Renderer::DisableDepthTest(); // direct render texture no need depth

        // LOG_DEBUG("camera {}",cameraUniformDeferredPass);
        programDeferredLight.Validate();
        Renderer::Clear();
        Renderer::Draw(planeVAO.GetIndexBuffer()->GetCount());

        programDeferredLight.Unbind();
        deferredLightFbo.Unbind();
#pragma endregion

        // screen space
#pragma region screen space
        Renderer::DisableDepthTest(); // direct render texture no need depth
        programScreen.Bind();
        screenAlbedo.Bind(ALBEDO);
        screenNormal.Bind(NORMAL);
        screenPosition.Bind(POSITION);
        screenEmission.Bind(EMISSION);
        screenDepth.Bind(DEPTH);

        screenLight.Bind(LIGHTING);
        screenVolume.Bind(VOLUME);

        programScreen.Validate();
        plane.Draw();
#pragma endregion

#pragma region GUI
        Renderer::DisableDepthTest();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        float framerate = ImGui::GetIO().Framerate;

        ImGui::Begin("Debug Info");
        ImGui::Text("%.1f FPS", framerate);
        ImGui::Text("(%d, %d)", (int)delta.x, (int)delta.y);
        ImGui::Text("(%d, %d)", (int)window.GetScrollOffset().x,
                    (int)window.GetScrollOffset().y);
        ImGui::End();

        transformSliders[0].Update();
        transformSliders[1].Update();

        transformSliders[2].Update();

        lightSliders[0].Update();

        transformSliders[3].Update();

        lightSliders[1].Update();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#pragma endregion

        // TODO: Figure this out and put it in `Window` class
        // glfwSwapInterval(0);
        glfwSwapBuffers(window.GetWindow());
        window.PollEvents();
    } while (!window.ShouldClose());

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
