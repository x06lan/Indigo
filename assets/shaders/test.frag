#version 450 core
layout(location = 0) in vec2 uv;
layout(location = 0) out vec4 outColor;

void main() {
    vec3 fragColor = vec3(uv.x,uv.y ,0.0);
    outColor = vec4(fragColor, 1.0);
}

