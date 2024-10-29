#version 450 core
layout(location = 0) in vec3 vertPosition;
layout(location = 1) in vec2 vertUV;

layout(location = 0) out vec2 UV;

void main() {
    gl_Position = vec4(vertPosition, 1.0);

    UV = vertUV;
}
