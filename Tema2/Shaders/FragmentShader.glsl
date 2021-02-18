#version 330

// get values from fragment shader
in vec3 frag_color;
in vec3 frag_normal;
in vec3 frag_position;
in vec2 frag_coord;

layout(location = 0) out vec4 out_color;
layout(location = 1) out vec4 out_normal;
layout(location = 2) out vec4 out_position;
layout(location = 3) out vec3 out_coord;

void main()
{
	// write pixel out color
	out_color = vec4(frag_color, 1);
	out_normal = vec4(frag_normal, 1);
	out_position = vec4(frag_position, 1);
	out_coord = vec3(frag_coord, 1);
	
}