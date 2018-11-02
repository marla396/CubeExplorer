layout(location = 0) in vec3 position;
layout(location = 1) in vec2 tex_coords;

out vec2 pass_tex_coords;

uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;

uniform vec4 clip_plane;

void main(void){

	vec4 world_position = model_matrix * vec4(position, 1.0);

	gl_ClipDistance[0] = dot(world_position, clip_plane);

	gl_Position = projection_matrix * view_matrix * world_position;
	pass_tex_coords = tex_coords;
}