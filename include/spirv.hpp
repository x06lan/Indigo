#ifndef SPIRV_HPP
#define SPIRV_HPP

#include "pch.hpp"

#include <unordered_map>

#include "program.hpp"
#include "scene.hpp"
#include "frame_buffer.hpp"
#include "uniform_buffer.hpp"

class ScreenRenderer {
public:
    struct MVP {
        glm::mat4 model;
        glm::mat4 viewProjection;
    };

    ScreenRenderer(int width, int height);

    void Init();

    void Render();

    void SetWidth(int width);
    void SetHeight(int height);

private:
    Program m_Compositor;

    VertexArray m_Screen;
    int m_Width;
    int m_Height;
};
#endif
