#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (location = 0) in  vec4 vVertex;
layout (location = 1) in  vec4 vNormal;
layout (location = 2) in  vec2 texCoords;

struct LightUBO{
	vec4 position;
	vec4 diffuse;
};

layout(binding = 0) uniform CameraUBO {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(binding = 1) uniform GlobalLightingUBO {
	LightUBO lights[10];
	int numLights;
} glights;

layout (location = 0) out vec3 vertNormal;
layout (location = 1) out vec3 eyeDir; 
layout (location = 2) out vec2 fragTextCords;
layout (location = 3) out vec3 vertPos;

void main() {
	fragTextCords = texCoords;
	mat3 normalMatrix = mat3(inverse(transpose(ubo.model)));
	vertNormal = normalize(normalMatrix * vNormal.xyz); /// Rotate the normal to the correct orientation 
	vertPos = vec3(ubo.view * ubo.model * vVertex); /// This is the position of the vertex from the origin
	vec3 vertDir = normalize(vertPos);
	eyeDir = -vertDir;
	
	gl_Position =  ubo.proj * ubo.view * ubo.model * vVertex; 
}
