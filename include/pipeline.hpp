#ifndef PIPELINE_HPP
#define PIPELINE_HPP

#include "pch.hpp"

#include "program.hpp"
#include "renderer.hpp"
#include "scene.hpp"
#include "frame_buffer.hpp"
#include "uniform_buffer.hpp"

class Pipeline {
public:
    enum Pass {
        // basic
        ALBEDO,
        EMISSION,
        NORMAL,
        ARM,
        POSITION,
        DEPTH,

        //
        REFLECT,

        // lighting
        LIGHTING,
        VOLUME,

        // shadow
        POINT_SHADOW, // Must be last because cube map stuff
        DIRECTION_SHADOW,

        SCREEN
    };
    struct MVP {
        glm::mat4 model;
        glm::mat4 viewProjection;
    };

    Pipeline(int width, int height);

    void Render(Scene scene);
    void Init();

private:
    void ShadowPass(Scene scene);
    void BasePass(Scene scene);
    void LightPass(Scene scene);
    void CompositorPass();

private:
    std::shared_ptr<Program> m_PointLightShadow;
    std::shared_ptr<Program> m_Basic;
    std::shared_ptr<Program> m_Light;
    std::shared_ptr<Program> m_Compositor;

    FrameBuffer m_ShadowFBO;
    FrameBuffer m_BasicPassFBO;
    FrameBuffer m_LightPassFBO;
    FrameBuffer m_CompositorFBO;

    std::vector<std::shared_ptr<UniformBuffer>> m_UBOs;
    std::map<int, std::shared_ptr<Texture>> m_Passes;

    VertexArray m_Screen;
    int m_Width = 1280;
    int m_Height = 720;
};
#endif