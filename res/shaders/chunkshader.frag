in vec2 tex_coords_fs;
in vec3 normal_fs;
in vec4 shadow_coords;
in vec4 world_position;

out vec4 color;

uniform sampler2D tex_unit0; //block_texture
uniform sampler2D tex_unit1; //shadow_map

uniform vec3 light_position;
uniform vec3 light_color;

const vec2 poisson_disk[4] = vec2[](
  vec2( -0.94201624, -0.39906216 ),
  vec2( 0.94558609, -0.76890725 ),
  vec2( -0.094184101, -0.92938870 ),
  vec2( 0.34495938, 0.29387760 )
);

void main(void){

	vec3 ndc = (shadow_coords.xyz / shadow_coords.w) / 2.0 + 0.5; 

	float occlusion = 1.0;

	for (int i = 0; i < 4; i++){
		if (texture(tex_unit1, ndc.xy + poisson_disk[i]/3500.0).z + 0.005 < ndc.z){
			occlusion -= 0.15;
		}
	}

	occlusion = max(occlusion, 0.4);

	float diffuse = max(dot(normal_fs, normalize(light_position - world_position.xyz)), 0.2);

	color = vec4(light_color, 1.0) * diffuse * occlusion * texture(tex_unit0, tex_coords_fs);
	color.a = 1.0;
}