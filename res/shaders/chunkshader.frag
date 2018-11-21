#include "ext/shadows.glsl"

in vec2 tex_coords_fs;
in vec3 normal_fs;
in vec4 shadow_coords_low;
in vec4 shadow_coords_high;
in vec4 world_position;

layout(location = 0) out vec4 g_color;
layout(location = 1) out vec4 g_position;
layout(location = 2) out vec3 g_normal;


uniform sampler2D tex_unit0; //block_texture
uniform sampler2D tex_unit1; //shadow_map_low
uniform sampler2D tex_unit2; //shadow_map_high

uniform vec3 light_position;
uniform vec3 light_color;
uniform vec3 camera_position;

void main(void){

	vec3 ndc_low = project_shadow_coords(shadow_coords_low);
	vec3 ndc_high = project_shadow_coords(shadow_coords_high);

	float occlusion = get_shadow_occlusion(tex_unit1, ndc_low, tex_unit2, ndc_high, length(camera_position - world_position.xyz), 0.007);

	float diffuse = max(dot(normal_fs, normalize(light_position - world_position.xyz)), 0.2);

	g_color = vec4(light_color, 1.0) * texture(tex_unit0, tex_coords_fs);

	g_color.xyz *= (diffuse * occlusion);
	g_position = world_position;
	g_normal = normal_fs;
}