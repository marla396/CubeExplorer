layout(location = 0) in vec2 position;
layout(location = 1) in vec2 tex_coords;

uniform mat4 model_matrix;

out vec3 world_position_tcs;
out vec2 tex_coords_tcs;

void main(void){
	world_position_tcs = (model_matrix * vec4(position.x, 0.0, position.y, 1.0)).xyz;
	tex_coords_tcs = tex_coords;
}