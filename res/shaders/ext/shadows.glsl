vec3 project_shadow_coords(vec4 shadow_coords){
	return (shadow_coords.xyz / shadow_coords.w) / 2.0 + 0.5; 
}

bool test_shadow_map(sampler2D shadow_map, vec3 shadow_coords, float epsilon){
	return texture(shadow_map, shadow_coords.xy).r + epsilon < shadow_coords.z;
}

float get_shadow_occlusion(sampler2D shadow_map_low, vec3 shadow_coords_low, 
							sampler2D shadow_map_high, vec3 shadow_coords_high, 
							float light_distance, float epsilon){

	if (light_distance <= LIGHT_HIGH_DISTANCE){
	
		if (test_shadow_map(shadow_map_high, shadow_coords_high, SHADOW_EPSILON_HIGH)){
			return 0.5;
		}
	} else{
	
		if (test_shadow_map(shadow_map_low, shadow_coords_low, SHADOW_EPSILON_LOW)){
			return 0.5;
		}
	}
	
	
	
	

	return 1.0;
}