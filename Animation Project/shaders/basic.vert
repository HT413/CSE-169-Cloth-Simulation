// A basic vertex shader.
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

uniform mat4 projection;
uniform mat4 modelview;

out vec4 Vertex;
out vec3 Normal;

void main(){
	gl_Position = projection * modelview * vec4(position, 1.0f);
	Vertex = modelview * vec4(position, 1.0f);
	Normal = transpose(inverse(mat3(modelview))) * normalize(normal);
}