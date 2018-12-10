#include "ext/shadows_frag.glsl"

out vec4 color;

in vec3 world_position_frag;
in vec2 tex_coords_frag;
in vec4 clip_space;
in vec3 tangent;

uniform sampler2D tex_unit0; //displacement_map dy
uniform sampler2D tex_unit2; //reflection_texture
uniform sampler2D tex_unit3; //refraction_texture
uniform sampler2D tex_unit4; //normal_map
uniform sampler2D tex_unit5; //displacement map dx
uniform sampler2D tex_unit6; //displacement map dz
uniform sampler2D tex_unit7; //dudv_map

uniform vec3 light_position;
uniform vec3 light_color;

uniform mat4 view_matrix;
uniform mat4 projection_matrix;

uniform float displacement_factor;
uniform vec3 camera_position;

float get_jacobian_det(vec2 coords){
	vec2 ts = 1.0 / textureSize(tex_unit0, 0);

	vec2 du = vec2(ts.x, 0.0);
	vec2 dv = vec2(0.0, ts.y);

	float dxdu = texture(tex_unit5, coords + du).r - texture(tex_unit5, coords - du).r;
	float dxdv = texture(tex_unit5, coords + dv).r - texture(tex_unit5, coords - dv).r;
	
	float dzdu = texture(tex_unit6, coords + du).r - texture(tex_unit6, coords - du).r;
	float dzdv = texture(tex_unit6, coords + dv).r - texture(tex_unit6, coords - dv).r;

	return dzdu * dxdv - dxdu * dzdv;
}

void main(void){

	vec2 ndc = (clip_space.xy / clip_space.w) / 2.0 + 0.5;

	vec2 reflect_coords = vec2(ndc.x, -ndc.y);
	vec2 refract_coords = vec2(ndc.x, ndc.y);

	vec3 normal = texture(tex_unit4, tex_coords_frag).rbg;

	float dist = length(camera_position - world_position_frag);

	float attenuation = clamp(-dist/projection_depth.y + 1,0.0,1.0);
	vec3 bitangent = normalize(cross(tangent, normal));
	mat3 TBN = mat3(tangent,bitangent,normal);
	vec3 bump_normal = texture(tex_unit4, tex_coords_frag * 8.0).rgb;
	bump_normal.z *= 4.0;
	bump_normal.xy *= attenuation;
	bump_normal = normalize(bump_normal);
	normal = normalize(TBN * bump_normal);


	vec2 dudv = texture(tex_unit7, tex_coords_frag).rg * displacement_factor;

	reflect_coords += dudv;
	refract_coords += dudv;

	reflect_coords.x = clamp(reflect_coords.x, 0.001, 0.999);
	reflect_coords.y = clamp(reflect_coords.y, -0.999, -0.001);

	refract_coords = clamp(refract_coords, 0.001, 0.999);

	vec4 reflection = texture(tex_unit2, reflect_coords);
	vec4 refraction = texture(tex_unit3, refract_coords);

	vec3 view_vector = normalize(camera_position - world_position_frag);
	float refractive_factor = dot(view_vector, vec3(0.0, 1.0, 0.0));
	refractive_factor = max(0.0, pow(refractive_factor, 4.0));

	vec3 reflected_light = reflect(normalize(world_position_frag - light_position), normal);
	float specular = max(dot(reflected_light, view_vector), 0.0);
	specular = pow(specular, 45.0);
	vec4 shade = vec4(0.8 * specular * light_color, 0.0);

	if (refractive_factor > 0.0)
		color = mix(reflection, refraction, refractive_factor);
	else
		color = refraction;

	float wave_break = 0.0f;
	float jac = get_jacobian_det(tex_coords_frag);
	if (jac < 0.1){
		wave_break = 3.0 * pow(abs(jac), 3.0);
	}

	float shadow_occlusion = get_shadow_occlusion();

	if (shadow_occlusion < 1.0){
		shade = vec4(0.0);
	}


	color = mix(shadow_occlusion * color, vec4(0.0, 0.15, 0.25, 1.0) + shade, 0.4);
	color = color + vec4(wave_break, wave_break, wave_break, 0.0);
	color.a = 1.0;
}
