#version 450

layout (location = 0) in vec3 VertexPosition;

layout(binding=0) uniform sampler2D nSampler;
layout(binding=1) uniform sampler2D uSampler; // base texture

uniform int uSize;

void main()
{	
	vec2 vTexCoords = vec2 (VertexPosition.x, VertexPosition.z) / uSize;
	float height = texture2D(uSampler, vTexCoords).z * 50.0;
	gl_Position = vec4 (VertexPosition.x, height, VertexPosition.z, 1.0);
}
