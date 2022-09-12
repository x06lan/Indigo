#ifndef LIGHT_HPP
#define LIGHT_HPP

#include "pch.hpp"
#include "transform.hpp"

enum LightType : int {
    point = 0,
    spot = 1,
    direction = 2,
};
struct LightData {
    TransformData transform_Data;
    glm::vec3 light_Color;
    float radius;
    float power;
    LightType light_Type;
};
class Light {
public:
    Light(glm::vec3 light_Color = glm::vec3(1.0f), float radius = 1.0f,
          float power = 1.0f, LightType light_Type = LightType::point);

    void SetLightColor(glm::vec3 light_Color);
    void SetRadius(float radius);
    void SetPower(float power);
    void SetLightType(LightType light_Type);

    glm::vec3 GetLightColor() const { return m_LightColor; };
    float GetRadius() const { return m_Radius; };
    float GetPower() const { return m_Power; };
    LightType GetLightType() const { return m_LightType; };

    LightData GetLightData();

    Transform m_Transform;

private:
    glm::vec3 m_LightColor;
    float m_Radius = 1.0f;
    float m_Power = 1.0f;
    LightType m_LightType = LightType::point;
};

#endif