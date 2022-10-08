#include "transform.hpp"

#include "log.hpp"

Transform::Transform(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
    : m_Position(position), m_Rotation(rotation), m_Scale(scale){};

void Transform::SetTransform(glm::mat4 transform) {
    m_Transform = transform;
    // position
    m_Position.x = transform[0][3];
    m_Position.y = transform[1][3];
    m_Position.z = transform[2][3];
    // scale
    m_Scale.x = transform[0][0];
    m_Scale.y = transform[1][1];
    m_Scale.z = transform[2][2];

    // rotation is bitch form
    // https://www.reddit.com/r/opengl/comments/sih6lc/4x4_matrix_to_position_rotation_and_scale/
    const glm::vec3 left =
        glm::normalize(glm::vec3(transform[0])); // Normalized left axis
    const glm::vec3 up =
        glm::normalize(glm::vec3(transform[1])); // Normalized up axis
    const glm::vec3 forward =
        glm::normalize(glm::vec3(transform[2])); // Normalized forward axis

    // Obtain the "unscaled" transform matrix
    glm::mat4 m(0.0f);
    m[0][0] = left.x;
    m[0][1] = left.y;
    m[0][2] = left.z;

    m[1][0] = up.x;
    m[1][1] = up.y;
    m[1][2] = up.z;

    m[2][0] = forward.x;
    m[2][1] = forward.y;
    m[2][2] = forward.z;

    glm::vec3 rot;
    rot.x = atan2f(m[1][2], m[2][2]);
    rot.y = atan2f(-m[0][2], sqrtf(m[1][2] * m[1][2] + m[2][2] * m[2][2]));
    rot.z = atan2f(m[0][1], m[0][0]);
    rot = glm::degrees(rot); // Convert to degrees, or you could multiply it by
    m_Rotation = rot;
}

glm::mat4 Transform::RotationMat(glm::mat4 transform) {
    transform = glm::rotate(transform, glm::radians(m_Rotation.x),
                            glm::vec3(1.f, 0.f, 0.f));
    transform = glm::rotate(transform, glm::radians(m_Rotation.y),
                            glm::vec3(0.f, 1.f, 0.f));
    transform = glm::rotate(transform, glm::radians(m_Rotation.z),
                            glm::vec3(0.f, 0.f, 1.f));
    return transform;
}

glm::mat4 Transform::UpdateMat() {
    glm::mat4 transform = glm::mat4(1.f);
    // transform = glm::translate(transform, glm::vec3(0.f));
    transform = glm::translate(transform, m_Position);
    transform = RotationMat(transform);
    transform = glm::scale(transform, m_Scale);
    m_Transform = transform;
    return transform;
}

glm::mat4 Transform::GetTransform() {
    UpdateMat();

    return m_Transform;
};
glm::vec3 Transform::GetDirection() {
    glm::vec4 direction = glm::vec4(0, 1, 0, 1);
    glm::mat4 transform = glm::mat4(1.f);
    transform = glm::translate(transform, glm::vec3(0));
    transform = glm::scale(transform, glm::vec3(1));
    transform = RotationMat(transform);
    direction = transform * direction;
    return glm::vec3(direction);
};

glm::mat4 Transform::GetDirection4() {
    glm::mat4 transform = glm::mat4(1.f);
    transform = glm::translate(transform, glm::vec3(0));
    transform = RotationMat(transform);

    return transform;
};
TransformData Transform::GetTransformData() {
    UpdateMat();

    TransformData data = {
        m_Transform, m_Position, 0.0f,           m_Rotation, 0.0f,
        m_Scale,     0.0f,       GetDirection(), 0.0f,
    };

    return data;
}