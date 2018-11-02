out vec4 color;
in vec2 pass_tex_coords;

uniform sampler2D tex_unit;

void main(void){
	color = texture(tex_unit, pass_tex_coords);
}