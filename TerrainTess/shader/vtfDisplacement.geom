#version 450

layout(triangles) in;
layout (triangle_strip, max_vertices=3) out;

uniform mat3 NormalMatrix;
uniform mat4 MVP;
uniform mat4 ModelViewMatrix;

out vec3 VNormal;
out vec3 VPosition;
out vec3 VColor;

 void main()
 {

	vec3 A = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
	vec3 B = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;
	
	vec3 normal = NormalMatrix * normalize(cross(A,B));

	VNormal = normal;

	VPosition = vec3( ModelViewMatrix * gl_in[0].gl_Position );
	gl_Position = MVP * gl_in[0].gl_Position;
	EmitVertex();

	
	VPosition = vec3( ModelViewMatrix * gl_in[1].gl_Position );
	gl_Position = MVP * gl_in[1].gl_Position;
	EmitVertex();
	

	VPosition = vec3( ModelViewMatrix * gl_in[2].gl_Position );
	gl_Position = MVP * gl_in[2].gl_Position;
	EmitVertex();

	EndPrimitive();
}