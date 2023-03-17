#ifndef LIGHT_HPP
#define LIGHT_HPP

#include "pch.hpp"

#include "transform.hpp"
#include "texture.hpp"

struct LightData;

class Light {
public:
    enum Type {
        NONE,
        POINT,
        SPOT,
        DIRECTION,
        AMBIENT,
    };

    Light(Type type, glm::vec3 lightColor = glm::vec3(1.0f),
          float radius = 1.0f, float power = 1.0f);

    void SetLightColor(glm::vec3 color) { m_Color = color; }
    void SetRadius(float radius) { m_Radius = radius; }
    void SetPower(float power) { m_Power = power; }
    void SetLightType(Type lightType);
    void SetInner(float inner) { m_InnerCone = inner; }
    void SetOuter(float outer) { m_OuterCone = outer; }
    void SetShadowSize(int size) {
        m_TextureSize = size;
        if (m_ShadowTexture) {
            m_ShadowTexture->SetWidth(m_TextureSize);
            m_ShadowTexture->SetHeight(m_TextureSize);
        }
    }
    void SetCastShadow(bool castShadow) { m_CastShadow = castShadow; }

    glm::mat4 GetLightProjection() const;
    std::vector<glm::mat4> GetLightProjectionCube() const;

    Type GetType() const { return m_Type; };
    glm::vec3 GetColor() const { return m_Color; };

    float GetRadius() const { return m_Radius; };
    float GetPower() const { return m_Power; };

    float GetInnerCone() const { return m_InnerCone; };
    float GetOuterCone() const { return m_OuterCone; };

    float GetTextureSize() const { return m_TextureSize; };
    bool GetCastShadow() const { return m_CastShadow; };
    std::shared_ptr<Texture> GetShadowTexture() const {
        return m_ShadowTexture;
    };

    LightData GetLightData();
    Texture::Target GetShadowTarget();

    Transform &GetTransform() { return m_Transform; }
    const Transform &GetTransform() const { return m_Transform; }

    void SetTransform(Transform transform) { m_Transform = transform; }

private:
    Type m_Type;

    glm::vec3 m_Color;

    float m_Radius = 200.0f;
    float m_Power = 0.5f;

    float m_InnerCone = 20.0f;
    float m_OuterCone = 30.0f;

    float m_NearPlane = 1;
    float m_FarPlane = 1000.0f;

    Transform m_Transform;
    int m_TextureSize = 1024;
    bool m_CastShadow = true;
    std::shared_ptr<Texture> m_ShadowTexture = nullptr;
};

struct LightData {
    TransformData transform;

    glm::vec3 color;
    float radius;

    float power;
    Light::Type type;

    float innerCone;
    float outerCone;

    glm::mat4 projections[6];

    float nearPlane;
    float farPlane;
    // lazy to fix padding issues
    int castShadow;
    float pad1;
};

#endif