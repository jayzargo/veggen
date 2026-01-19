#version 460 core

layout (location = 0) in vec3 aPos;

uniform mat4 P;

void main( void )
{	
	gl_Position = P * vec4(aPos.x, aPos.y, aPos.z, 1.0);
}