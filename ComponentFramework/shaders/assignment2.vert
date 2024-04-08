#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (location = 0) in  vec4 vVertex;
layout (location = 1) in  vec4 vNormal;
layout (location = 2) in  vec2 texCoords;

layout(binding = 0) uniform CameraUBO {
    mat4 view;
    mat4 proj;
	vec3 viewPos;
} ubo;

layout( push_constant ) uniform ModelMatrixPushConst {
	mat4 modelMatrix;
    mat4 normalMatrix;
} Matrix;

layout (location = 0) out vec3 vertNormal;
layout (location = 1) out vec2 fragTextCords;
layout (location = 2) out vec3 FragPos;
layout (location = 3) out vec3 viewPos;

void main() {
	fragTextCords = texCoords;
	vertNormal = vNormal.xyz;
	FragPos = vec3(Matrix.modelMatrix * vVertex);
	viewPos = ubo.viewPos;
	
	gl_Position =  ubo.proj * ubo.view * Matrix.modelMatrix * vVertex; 
}
