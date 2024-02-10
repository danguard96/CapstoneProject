#version 450
#extension GL_ARB_separate_shader_objects : enable
layout(binding = 2) uniform sampler2D texSampler;

layout (location = 0) in vec3 vertNormal;
layout (location = 1) in vec3 eyeDir; 
layout (location = 2) in vec2 fragTextCords;
layout (location = 3) in vec3 vertPos;

struct LightUBO{
	vec4 position;
	vec4 diffuse;
};

layout(binding = 1) uniform GlobalLightingUBO {
	LightUBO lights[10];
	int numLights;
} glights;

layout (location = 0) out vec4 fragColor;

//Phong
vec4 CalcPointLight(LightUBO light)
{
    vec4 ks = light.diffuse;
	vec4 ka = vec4(0.2, 0.2, 0.2, 1.0);
	vec4 kt = texture(texSampler, fragTextCords);
	vec3 lightDir = normalize(light.position.xyz - vertPos);
	vec3 vertDir = normalize(vertPos);
	float diff = max(dot(vertNormal, lightDir), 0.0);
	vec3 reflection = normalize(reflect(-lightDir, vertNormal));
	float spec = max(dot(-vertDir, reflection), 0.0);
	if(diff > 0.0){
		spec = pow(spec,14.0);
	}
	return 0.3 * kt + (diff * (light.diffuse + kt)) + (spec * ks);
} 

void main() { 
	vec4 sum = vec4(0.0,0.0,0.0,0.0);
	for(int i = 0; i < glights.numLights; i++){
		sum += CalcPointLight(glights.lights[i]);
	}
	fragColor = sum;	
} 

