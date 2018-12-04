#include "ext/shadows_frag.glsl"

in vec2 tex_coords_fs;
in vec3 normal_fs;
in vec4 world_position;

out vec4 color;

uniform sampler2D tex_unit0; //block_texture

uniform vec3 light_position;
uniform vec3 light_color;

void main(void){


	float occlusion = get_shadow_occlusion();

	float diffuse = max(dot(normal_fs, normalize(light_position - world_position.xyz)), 0.2);

	color = vec4(light_color, 1.0) * texture(tex_unit0, tex_coords_fs);

	color.xyz *= (diffuse * occlusion);
}