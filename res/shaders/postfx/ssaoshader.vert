layout(location = 0) in vec2 position;
layout(location = 1) in vec2 tex_coords;

out vec2 tex_coords_fs;

void main(void){
	tex_coords_fs = tex_coords;
	gl_Position = vec4(position.x, position.y, 0.0, 1.0);
}