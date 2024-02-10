#version 410
in vec3 vertNormal;
in vec3 lightDir;
in vec3 vertDir;
in vec2 uvCoords;
out vec4 fragColor;

uniform sampler2D myTexture; 

void main() { 
	float spec = 0.0;
	const vec4 specLightColor = vec4(0.2, 0.2, 0.2, 0.0);
	float diff = max(dot(vertNormal, lightDir), 0.0);
	vec4 textureColor = texture(myTexture, uvCoords);
	if(diff > 0.0){
		vec3 reflection = normalize(reflect(lightDir, vertNormal));
		spec = max(dot(vertDir, reflection), 0.0);
		spec = pow(spec,12.0);
	}
	vec4 ka = 0.1 * textureColor ;
	fragColor =  ka + (diff * textureColor)  + (spec * specLightColor);	
} 

