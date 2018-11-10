in vec2 tex_coords_fs;
in vec3 normal_fs;
in vec4 shadow_coords;
in vec4 world_position;

out vec4 color;

uniform sampler2D tex_unit0; //block_texture
uniform sampler2D tex_unit1; //shadow_map

uniform vec3 light_position;
uniform vec3 light_color;

void main(void){

	vec3 ndc = (shadow_coords.xyz / shadow_coords.w) / 2.0 + 0.5; 

	float occlusion = 1.0;

	if (texture(tex_unit1, ndc.xy).r + 0.005 < ndc.z){
		occlusion -= 0.5;
	}

	float diffuse = max(dot(normal_fs, normalize(world_position.xyz - light_position)), 0.0);

	color = vec4(light_color, 1.0) * diffuse * occlusion * texture(tex_unit0, tex_coords_fs);
	color.a = 1.0;
}