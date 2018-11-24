layout(location = 0) out float depth;

in vec2 tex_coords_fs;

uniform sampler2D tex_unit0; //block_texture

void main(void){

	if (texture(tex_unit0, tex_coords_fs).a != 1.0)
		discard;
	
	depth = gl_FragCoord.z;
}
