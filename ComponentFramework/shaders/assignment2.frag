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
	float frame;
	bool distort;
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

//pink noise

float mod289(float x){return x - floor(x * (1.0 / 289.0)) * 289.0;}
vec4 mod289(vec4 x){return x - floor(x * (1.0 / 289.0)) * 289.0;}
vec4 perm(vec4 x){return mod289(((x * 34.0) + 1.0) * x);}

float noise(vec3 p){
    vec3 a = floor(p);
    vec3 d = p - a;
    d = d * d * (3.0 - 2.0 * d);

    vec4 b = a.xxyy + vec4(0.0, 1.0, 0.0, 1.0);
    vec4 k1 = perm(b.xyxy);
    vec4 k2 = perm(k1.xyxy + b.zzww);

    vec4 c = k2 + a.zzzz;
    vec4 k3 = perm(c);
    vec4 k4 = perm(c + 1.0);

    vec4 o1 = fract(k3 * (1.0 / 41.0));
    vec4 o2 = fract(k4 * (1.0 / 41.0));

    vec4 o3 = o2 * d.z + o1 * (1.0 - d.z);
    vec2 o4 = o3.yw * d.x + o3.xz * (1.0 - d.x);

    return o4.y * d.y + o4.x * (1.0 - d.y);
}

void main() { 
	vec4 sum = vec4(0.0,0.0,0.0,0.0);
	for(int i = 0; i < glights.numLights; i++){
		sum += CalcPointLight(glights.lights[i]);
	}
	fragColor = sum * (glights.distort ? noise(vec3(vertPos.x,vertPos.y,glights.frame)) : 1);	
} 

