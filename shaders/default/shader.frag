#version 330 core

in vec3 fragPos;
in vec2 o_texCoord;
in vec3 Normal;

uniform vec3 xyz;      // light position
uniform vec3 xyzColor; // light color
uniform vec3 xyzView;  // camera position
uniform float u_modifier;  // materials attempt


out vec4 color;
uniform sampler2D texture1;

// PHONG LIGHT
vec3 phong(vec3 pos, vec3 color,vec3 view){
	// ambient
	float strength = u_modifier;
	vec3 ambient   = strength * color;

	// diffuse
	vec3  norm     = normalize(Normal);
	vec3  lightDir = normalize(pos - fragPos);
	float diff     = max(dot(norm, lightDir), 0.0);
	vec3  diffuse  = diff * color;

	// specular
	float specularStrength = 0.3;
	vec3  viewDir          = normalize(view - fragPos);
	vec3  reflectDir       = reflect(-lightDir, norm);
	float spec             = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3  specular         = specularStrength * spec * color;

    // total (phong)
	return vec3(ambient + diffuse + specular);
}

void main() {
    vec3 result = phong(xyz, xyzColor, xyzView) * vec3(texture(texture1, o_texCoord));
	color = vec4(result, 1.0);
	// color = vec4(ambient*fragColor, 1.0); // solo ambient light
}
