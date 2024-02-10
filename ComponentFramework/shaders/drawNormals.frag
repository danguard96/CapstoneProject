#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (binding = 1) uniform NormalUBO {
    float plength;
    vec4 colour;
} normalUBO;

layout(location = 0) out vec4 fragColor;

void main() {
    fragColor = vec4(0.0, 1.0, 1.0, 0.0);
}