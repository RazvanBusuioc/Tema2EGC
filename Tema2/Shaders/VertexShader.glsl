#version 330

// get vertex attributes from each location
layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 tex_coord;
layout(location = 3) in vec3 v_color;


// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

// output values to fragment shader
out vec3 frag_color;
out vec3 frag_normal;
out vec3 frag_position;
out vec2 frag_coord;

void main()
{
	// send output to fragment shader
	frag_color = v_color;
	frag_normal = v_normal;
	frag_position = v_position;
	frag_coord = tex_coord;

	// compute gl_Position
	gl_Position = Projection * View * Model * vec4(v_position, 1.0f);
}
