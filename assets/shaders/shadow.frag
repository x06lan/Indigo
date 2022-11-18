#version 460 core

#define LIGHT_NUMBER 2

in vec3 geoPosition;
in vec3 worldPosition;

// out vec4 color;

struct TransformData {
    mat4 transform;
    vec3 position;
    float pad1;
    vec3 rotation;
    float pad2;
    vec3 scale;
    float pad3;
    vec3 direction;
    float pad4;
};

struct LightData {
    TransformData transform;

    vec3 lightColor;
    float radius;
    float power;
    int lightType;
    float innerCone;
    float outerCone;
};

struct MaterialData {
    vec3 baseColor;
    float maxShine;
    // vec3 normal;
};

layout(std140, binding = 1) uniform Materials {
    MaterialData material;
};

uniform sampler2D texture1; // samplers are opaque types and
void main() {
    vec3 color3 = vec3(0.);
    gl_FragDepth = gl_FragCoord.z;
    // color = vec4(vec3(gl_FragCoord.z), 1.0);
    // color = vec4(vec3(0,0,1), 1.0);
}