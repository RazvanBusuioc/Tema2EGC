#version 330

uniform vec3 curr_points_color2D;

layout(location = 0) out vec4 out_color;

void main()
{
	out_color = vec4(abs(curr_points_color2D) , 1);
}