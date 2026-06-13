#version 330 core

layout(location = 0) in vec3 localPos;
layout(location = 1) in vec2 localUV;

layout(location = 2) in vec4 instance_color;
layout(location = 3) in mat4 instance_model;

layout (std140) uniform MainBlock{
	mat4 pv;
	float t;
	float dt;
	vec2 res;
};

out vec4 color;

void main(){
	gl_Position = pv * instance_model * vec4(localPos,1.0);
	color = instance_color;
}