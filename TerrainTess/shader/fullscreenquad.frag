#version 450 core

in vec2 texcoord;
out vec4 outputColor;
uniform sampler2D uSampler;

void main()
{
	//if(texcoord.x > 0.0 && texcoord.y > 0.0 && texcoord.x < 1.0 && texcoord.y < 1.0)
	outputColor = texture2D(uSampler, texcoord);
	//outputColor = vec4(1.0, 0.0, 0.0, 1.0);
}