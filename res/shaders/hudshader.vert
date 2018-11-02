layout(location = 0) in vec2 position;
layout(location = 1) in vec2 tex_coords;

out vec2 pass_tex_coords;

uniform mat4 transformation_matrix;

void main(void){
	gl_Position = transformation_matrix * vec4(position.x, position.y, 0.0, 1.0);
	pass_tex_coords = tex_coords;
}