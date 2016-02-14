#version 400

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexColor;
layout (location = 2) in vec3 VertexNormal;

out vec3 VPosition;
out vec3 VNormal;
out vec3 VColor;

uniform mat4 MVP;
uniform mat3 NormalMatrix;
uniform mat4 ModelViewMatrix;

void main()
{
	VColor = VertexColor;
	VNormal = normalize( NormalMatrix * VertexNormal);
	VPosition = vec3( ModelViewMatrix * vec4(VertexPosition, 1.0) ); // vertex pos on eye 

    gl_Position = MVP * vec4(VertexPosition,1.0);
}
