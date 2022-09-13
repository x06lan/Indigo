#include "glm/fwd.hpp"
#include "pch.hpp"

#include "log.hpp"
#include "window.hpp"
#include "renderer.hpp"
#include "program.hpp"
#include "camera.hpp"
#include "importer.hpp"
#include "vertex_array.hpp"
#include "vertex_buffer.hpp"
#include "index_buffer.hpp"
#include "uniform_buffer.hpp"
#include "texture.hpp"
#include "transform.hpp"
#include "light.hpp"

#pragma pack(16) // std140 layout pads by multiple of 16
struct Matrices {
    glm::mat4 model;
    glm::mat4 viewProjection;
};

struct Material {
    glm::vec3 baseColor;
    float maxShine;
};

int main(int, char **) {
    Log::Init();

    Window window;

    Renderer::Init();
    Renderer::ClearColor(0.102f, 0.02f, 0.478f, 1.0f);

    Program program("../assets/shaders/phong.vert",
                    "../assets/shaders/phong.frag");

    LightData light_info[1];
    UniformBuffer matrices(sizeof(Matrices), 0);
    UniformBuffer material_Data(sizeof(Material), 1);
    UniformBuffer light_data(sizeof(light_info), 2);

    Camera camera(45.0f, window.GetAspectRatio());

    Light light1(glm::vec3(1.0f));
    // begin model 1
    Transform model1Trans(glm::vec3(0));
    // glm::mat4 model1 = glm::mat4(1.0f);
    model1Trans.SetPosition(glm::vec3(2, 0, 0));
    glm::mat4 model1 = model1Trans.GetTransform();

    Material mat_color1 = {glm::vec3(0.8f, 0.5f, 0.0f), 100.0f};
    // model1 = glm::translate(model1, glm::vec3(2, 0, 0));

    Importer obj1("../assets/donut.obj");

    VertexArray vao1;

    vao1.AddVertexBuffer(
        std::make_shared<VertexBuffer>(obj1.GetVertices(), 3 * sizeof(float)));

    vao1.AddVertexBuffer(
        std::make_shared<VertexBuffer>(obj1.GetUVs(), 2 * sizeof(float)));

    vao1.AddVertexBuffer(
        std::make_shared<VertexBuffer>(obj1.GetNormals(), 3 * sizeof(float)));

    vao1.SetIndexBuffer(std::make_shared<IndexBuffer>(obj1.GetIndices()));
    // end model 1

    // begin model 2
    glm::mat4 model2 = glm::mat4(1.0f);
    Material mat_color2 = {glm::vec3(0.0f, 0.8f, 0.8f), 100.0f};
    model2 = glm::translate(model2, glm::vec3(-2, 0, 0));

    Importer obj2("../assets/suzanne.obj");

    VertexArray vao2;

    vao2.AddVertexBuffer(
        std::make_shared<VertexBuffer>(obj2.GetVertices(), 3 * sizeof(float)));

    vao2.AddVertexBuffer(
        std::make_shared<VertexBuffer>(obj2.GetUVs(), 2 * sizeof(float)));

    vao2.AddVertexBuffer(
        std::make_shared<VertexBuffer>(obj2.GetNormals(), 3 * sizeof(float)));

    vao2.SetIndexBuffer(std::make_shared<IndexBuffer>(obj2.GetIndices()));
    // end model 2

    Texture tex1("../assets/fabric.png");
    Texture tex2("../assets/uv.png");

    program.Bind();

    program.SetInt("texture1", 0);
    program.SetInt("texture2", 1);

    do {
        Renderer::Clear();
        vao1.Bind();

        model1Trans.SetRotation(model1Trans.GetRotation() +
                                glm::vec3(0.1, 0.0, 0));
        model1 = model1Trans.GetTransform();

        light_info[0] = light1.GetLightData();
        Matrices mat1;
        mat1.model = model1;
        mat1.viewProjection = camera.GetViewProjection();
        matrices.SetData(0, sizeof(mat1), &mat1);
        material_Data.SetData(0, sizeof(Material), &mat_color1);
        light_data.SetData(0, sizeof(light_info), &light_info);

        tex1.Bind(0);
        tex2.Bind(1);

        Renderer::Draw(vao1.GetIndexBuffer()->GetCount());
        vao2.Bind();

        model2 = glm::rotate(model2, glm::radians(-.1f), glm::vec3(0, 1, 0));

        Matrices mat2;
        mat2.model = model2;
        mat2.viewProjection = camera.GetViewProjection();
        matrices.SetData(0, sizeof(mat2), &mat2);
        material_Data.SetData(0, sizeof(Material), &mat_color2);

        tex2.Bind(0);
        tex1.Bind(1);

        Renderer::Draw(vao2.GetIndexBuffer()->GetCount());

        glfwSwapBuffers(window.GetWindow());
        glfwPollEvents();
    } while (!window.ShouldClose());
}
