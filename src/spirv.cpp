#include "spirv.hpp"

#include <iostream>
#include <random>

#include "renderer.hpp"

ScreenRenderer::ScreenRenderer(int width, int height)
    : m_Compositor("../assets/shaders/test.vert",
                   "../assets/shaders/test.frag"),
      m_Width(width), m_Height(height) {
    m_Screen.AddVertexBuffer(std::make_shared<VertexBuffer>(
        std::vector<float>{
            -1.0f, 1.0f,  //
            -1.0f, -1.0f, //
            1.0f, -1.0f,  //
            1.0f, 1.0f,   //
        },
        2 * sizeof(float)));

    // UV
    m_Screen.AddVertexBuffer(std::make_shared<VertexBuffer>(
        std::vector<float>{
            0.0f, 1.0f, //
            0.0f, 0.0f, //
            1.0f, 0.0f, //
            1.0f, 1.0f, //
        },
        2 * sizeof(float)));

    // Indices
    m_Screen.SetIndexBuffer(
        std::make_shared<IndexBuffer>(std::vector<unsigned int>{
            0, 1, 2, //
            0, 2, 3, //
        }));
}
void ScreenRenderer::SetWidth(int width) {
    m_Width = width;
    // UpdatePass();
}

void ScreenRenderer::SetHeight(int height) {
    m_Height = height;
    // UpdatePass();
}

void ScreenRenderer::Render() {
    Renderer::DisableDepthTest(); // direct render texture no need depth
    Renderer::EnableCullFace();

    glViewport(0, 0, m_Width, m_Height);
    m_Compositor.Bind();
    m_Screen.Bind();

    Renderer::Draw(m_Screen.GetIndexBuffer()->GetCount());

    m_Compositor.Unbind();
}