#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

const float amplitude1 = 0.01;
const float frequency1 = 4.0;
const float amplitude2 = 0.05;
const float frequency2 = 4.0;
const float amplitude3 = 0.05;
const float PI = 3.14159;
out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float time;
uniform int option;

void main()
{
	if(option == 1){
		  float distance = length(aPos);
		  float effect = amplitude1*sin(-PI*distance*frequency1+time);
		  gl_Position = projection*view*model*vec4(aPos.x+effect,aPos.y+effect, aPos.z,1);
		  TexCoords=vec2(aTexCoords.x+effect,aTexCoords.y+effect);
	  }
	if(option == 2){
		float distance = length(aPos);
		float effect = amplitude2*sin(-PI*distance*frequency2+4*time);
		gl_Position = projection*view*model*vec4(aPos.x,aPos.y, aPos.z+effect,1);
		TexCoords=vec2(aTexCoords.x,aTexCoords.y);
	}
	if(option == 3){
		float distance = length(aPos);
		float effect = amplitude3*sin(-PI*distance*frequency2+4*time);
		gl_Position = projection*view*model*vec4(aPos.x,aPos.y, aPos.z+effect,1);
		TexCoords=vec2(aTexCoords.x,aTexCoords.y);
	}

}