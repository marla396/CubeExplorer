in vec3 position;
in vec2 tex_coords;
out vec2 pass_tex_coords;

uniform mat4 projection_matrix;
uniform mat4 view_matrix;
uniform mat4 model_matrix;

void main(void)
{
	vec4 world_position = model_matrix * vec4(position, 1.0);

	gl_Position = projection_matrix * view_matrix * world_position;
	pass_tex_coords = tex_coords;
}