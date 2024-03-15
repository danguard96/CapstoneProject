#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (location = 0) in  vec4 vVertex;
layout (location = 1) in  vec4 vNormal;
layout (location = 2) in  vec2 texCoords;

layout(binding = 0) uniform CameraUBO {
    mat4 view;
    mat4 proj;
} ubo;

layout( push_constant ) uniform ModelMatrixPushConst {
	mat4 modelMatrix;
    mat4 normalMatrix;
} Matrix;

layout (location = 0) out vec3 vertNormal;
layout (location = 1) out vec3 eyeDir; 
layout (location = 2) out vec2 fragTextCords;
layout (location = 3) out vec3 vertPos;

void main() {
	fragTextCords = texCoords;
	vertNormal = normalize(mat3(Matrix.normalMatrix) * vNormal.xyz); /// Rotate the normal to the correct orientation 
	vertPos = vec3(ubo.view * Matrix.modelMatrix * vVertex); /// This is the position of the vertex from the origin
	vec3 vertDir = normalize(vertPos);
	eyeDir = -vertDir;
	
	gl_Position =  ubo.proj * ubo.view * Matrix.modelMatrix * vVertex; 
}
