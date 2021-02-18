#version 330

uniform vec3 color;

in float randomNr;

layout(location = 0) out vec4 out_color;

void main()
{
	out_color = vec4(abs(color) * randomNr, 1);
}