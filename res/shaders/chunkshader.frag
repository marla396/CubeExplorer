#include "ext/shadows_frag.glsl"

out vec4 color;
in vec2 tex_coords_fs;
in vec3 normal_fs;
in vec4 world_position;

uniform sampler2D tex_unit0; //block_texture

uniform vec3 light_position;
uniform vec3 light_color;

void main(void){

	color = texture(tex_unit0, tex_coords_fs);

	if (color.a == 0.0)
		discard;

	float occlusion = get_shadow_occlusion();

	float diffuse = max(1.2 * dot(normal_fs, normalize(light_position - world_position.xyz)), 0.3);

	color = vec4(light_color, 1.0) * texture(tex_unit0, tex_coords_fs);

	color.xyz *= (diffuse * occlusion);
}