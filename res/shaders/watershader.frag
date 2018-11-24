#include "ext/shadows.glsl"

out vec4 color;

in vec3 world_position_frag;
in vec2 tex_coords_frag;
in vec4 clip_space;
in vec4 shadow_coords_low;
in vec4 shadow_coords_high;

uniform sampler2D tex_unit0; //displacement_map
uniform sampler2D tex_unit2; //reflection_texture
uniform sampler2D tex_unit3; //refraction_texture
uniform sampler2D tex_unit4; //normal_map
uniform sampler2D tex_unit7; //dudv_map
uniform sampler2D tex_unit8; //shadow_map_low
uniform sampler2D tex_unit9; //shadow_map_high

uniform vec3 camera_position;
uniform vec3 light_position;
uniform vec3 light_color;

uniform mat4 view_matrix;
uniform mat4 projection_matrix;

uniform float displacement_factor;

void main(void){

	vec2 ndc = (clip_space.xy / clip_space.w) / 2.0 + 0.5;

	vec2 reflect_coords = vec2(ndc.x, -ndc.y);
	vec2 refract_coords = vec2(ndc.x, ndc.y);

	vec3 normal = texture(tex_unit4, tex_coords_frag).rbg;

	vec2 dudv = texture(tex_unit7, tex_coords_frag).rg * 0.05 * displacement_factor;

	normal = normalize(normal);

	reflect_coords += dudv;
	refract_coords += dudv;

	reflect_coords.x = clamp(reflect_coords.x, 0.001, 0.999);
	reflect_coords.y = clamp(reflect_coords.y, -0.999, -0.001);

	refract_coords = clamp(refract_coords, 0.001, 0.999);

	vec4 reflection = texture(tex_unit2, reflect_coords);
	vec4 refraction = texture(tex_unit3, refract_coords);

	vec3 view_vector = normalize(camera_position - world_position_frag);
	float refractive_factor = dot(view_vector, vec3(0.0, 1.0, 0.0));
	refractive_factor = max(0.0, pow(refractive_factor, 1.5));


	vec3 reflected_light = reflect(normalize(world_position_frag - light_position), normal);
	float specular = max(dot(reflected_light, view_vector), 0.0);
	float diffuse = 0.0;//max(dot(normalize(world_position_frag - light_position), normal), 0.0);
	specular = pow(specular, 10.0);
	vec4 shade = vec4((diffuse * 0.9 + 1.3 * specular) * light_color, 0.0);

	if (refractive_factor > 0.0)
		color = mix(reflection, refraction, refractive_factor);
	else
		color = refraction;



	//vec3 sndc = (shadow_coords.xyz / shadow_coords.w) * 0.5 + 0.5;
	vec3 sndc_low = project_shadow_coords(shadow_coords_low);
	vec3 sndc_high = project_shadow_coords(shadow_coords_high);
	 
	float shadow_occlusion = get_shadow_occlusion(tex_unit8, sndc_low, tex_unit9, sndc_high, length(world_position_frag - camera_position), 0.007);

	/*if (shadow_occlusion > 0.0){
		shade = vec4(0.0);
	}*/

	color = mix(shadow_occlusion * color, vec4(0.0, 0.15, 0.25, 1.0) + shade, 0.4);
	color = color * shadow_occlusion;
	color.a = 1.0;
}
