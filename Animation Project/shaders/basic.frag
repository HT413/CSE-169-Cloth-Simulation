// A basic fragment shader
#version 330 core

in vec4 Vertex;
in vec3 Normal;

out vec4 fragColor;

// Uniform variables
uniform vec3 camPos; // Camera position
uniform int isFloor; // Whether this is the floor or not

// Object material properties
vec4 ambient = vec4(0.15, 0.15, 0.15, 1.0);
vec4 diffuse = vec4(0.7, 0.7, 0.7, 1.0);
vec4 specular = vec4(1, 1, 1, 1.0);
int shininess = 1;

// Light properties
vec4 light1Pos = vec4(2, 0, 1, 0.0);
vec4 light1Color = vec4(0.5, 0.65, 1.0, 1.0);

vec4 ComputeLight (const in vec3 direction, const in vec4 lightcolor, const in vec3 normal, const in vec3 halfvec, const in vec4 mydiffuse, const in vec4 myspecular, const in int myshininess){
	float nDotL = dot(normal, direction);
	vec4 lambert = mydiffuse * lightcolor * max (nDotL, 0.0);
	float nDotH = dot(normal, halfvec);
	vec4 phong = myspecular * lightcolor * pow (max(nDotH, 0.0), myshininess);
	vec4 result = lambert + phong;
	return result;
}

void main(){
	vec4 finalColor;
	if(isFloor != 0){
	finalColor = vec4(1.0f, 0.9f, 0.8f, 1.0f);
	}
	else{
		finalColor = ambient;
		// Get the world position & normal
		vec3 pos = Vertex.xyz / Vertex.w;
		vec3 eyeDir = normalize(camPos - pos);
		vec3 normal = normalize(Normal);

		// Calculate light properties
		vec3 light1Dir = normalize(light1Pos.xyz); // Directional light
		vec3 halfVec1 = normalize(light1Dir + eyeDir);

		// Now calculate the lighting and add that to the final result
		vec4 color1 = ComputeLight(light1Dir, light1Color, normal, halfVec1, diffuse, specular, shininess);
		finalColor = finalColor + color1;
		vec4 color2 = ComputeLight(-light1Dir, light1Color, normal, -halfVec1, diffuse, specular, shininess);
		finalColor = finalColor + color2;
	}
	fragColor = finalColor;
}