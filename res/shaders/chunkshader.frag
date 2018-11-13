in vec2 tex_coords_fs;
in vec3 normal_fs;
in vec4 shadow_coords;
in vec4 world_position;

layout(location = 0) out vec4 g_color;
layout(location = 1) out vec3 g_position;
layout(location = 2) out vec3 g_normal;


uniform sampler2D tex_unit0; //block_texture
uniform sampler2D tex_unit1; //shadow_map

uniform vec3 light_position;
uniform vec3 light_color;

void main(void){

	vec3 ndc = (shadow_coords.xyz / shadow_coords.w) / 2.0 + 0.5; 

	float occlusion = 1.0;

	if (texture(tex_unit1, ndc.xy).r + 0.007 < ndc.z){
		occlusion -= 0.5;
	}

	float diffuse = max(dot(normal_fs, normalize(light_position - world_position.xyz)), 0.2);

	g_color = vec4(light_color, 1.0) * texture(tex_unit0, tex_coords_fs);

	g_color.xyz *= (diffuse * occlusion);
	g_position = world_position.xyz;
	g_normal = normal_fs;
}