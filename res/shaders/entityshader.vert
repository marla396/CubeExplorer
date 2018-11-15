layout(location = 0) in vec3 position;
layout(location = 1) in vec2 tex_coords;
layout(location = 2) in vec3 normal;

out vec2 tex_coords_fs;
out vec3 normal_fs;
out vec4 shadow_coords_low;
out vec4 shadow_coords_high;
out vec4 world_position;

uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;
uniform mat4 shadow_transform_low;
uniform mat4 shadow_transform_high;

uniform vec4 clip_plane;

void main(void){

	world_position = model_matrix * vec4(position, 1.0);

	gl_ClipDistance[0] = dot(world_position, clip_plane);

	shadow_coords_low = shadow_transform_low * world_position;
	shadow_coords_high = shadow_transform_high * world_position;

	gl_Position = projection_matrix * view_matrix * world_position;
	tex_coords_fs = tex_coords;
	normal_fs = normal;
}