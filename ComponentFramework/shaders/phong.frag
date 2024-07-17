#version 450
#extension GL_ARB_separate_shader_objects : enable
layout(binding = 2) uniform sampler2D texSampler;

layout (location = 0) in vec3 Normal;
layout (location = 1) in vec2 fragTextCords;
layout (location = 2) in vec3 FragPos;
layout (location = 3) in vec3 viewPos;

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
	vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

	vec3 lightDir = vec3(normalize(light.position - vec4(FragPos,1)));
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 reflectDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, reflectDir), 0.0), 5.0);
    float distance = length(vec3(light.position) - FragPos);
    float attenuation = 1.0 / distance;
    vec3 ambient  = vec3(0.5,0.5,0.5)  * vec3(texture(texSampler, fragTextCords));
    vec3 diffuse  = vec3(light.diffuse)  * diff * vec3(texture(texSampler, fragTextCords));
    vec3 specular = vec3(1.0,1.0,1.0) * spec * vec3(texture(texSampler, fragTextCords));
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return vec4(ambient + diffuse + specular, 1.0);
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
	fragColor = sum * (glights.distort ? noise(vec3(FragPos.x,FragPos.y,glights.frame)) : 1);	
} 

