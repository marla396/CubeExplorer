layout(location = 0) in vec3 position;
layout(location = 1) in vec2 tex_coords;

out vec2 pass_tex_coords;

uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;

void main(void){
	gl_Position = projection_matrix * view_matrix * model_matrix * vec4(position, 1.0);
	pass_tex_coords = tex_coords;
}