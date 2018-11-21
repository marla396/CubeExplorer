vec3 project_shadow_coords(vec4 shadow_coords){
	return (shadow_coords.xyz / shadow_coords.w) / 2.0 + 0.5; 
}

bool test_shadow_map(sampler2D shadow_map, vec3 shadow_coords, float epsilon){
	return texture(shadow_map, shadow_coords.xy).r + epsilon < shadow_coords.z;
}

vec2 poissonDisk[4] = vec2[](
  vec2( -0.94201624, -0.39906216 ),
  vec2( 0.94558609, -0.76890725 ),
  vec2( -0.094184101, -0.92938870 ),
  vec2( 0.34495938, 0.29387760 )
);


float get_shadow_value(sampler2D shadow_map, vec3 shadow_coords, float epsilon){
	
	vec2 offset = 1.0 / textureSize(shadow_map, 0);
	
	float occlusion = 1.0;

	for (int y = -1; y <= 1; y++){
		for (int x = -1; x <= 1; x++){
			vec3 offset = vec3(x * offset.x, y * offset.y, 0.0);
			
			if (test_shadow_map(shadow_map, shadow_coords + offset, epsilon)){
				occlusion -= (1.0 / 18.0);
			}
		}
	}

	/*for (int i = 0; i < 4; i++){
		if (test_shadow_map(shadow_map, shadow_coords + vec3(poissonDisk[i]/5600.0, 0.0), epsilon)){
			occlusion -= (1.0 / 8.0);
		}
	}*/

	/*if (test_shadow_map(shadow_map, shadow_coords + vec3(0.0, -2.0 * offset.y, 0.0), epsilon)){
		occlusion -= (1.0 / 26.0);
	}

	if (test_shadow_map(shadow_map, shadow_coords + vec3(0.0, 2.0 * offset.y, 0.0), epsilon)){
		occlusion -= (1.0 / 26.0);
	}

	if (test_shadow_map(shadow_map, shadow_coords + vec3(-2.0 * offset.x, 0.0, 0.0), epsilon)){
		occlusion -= (1.0 / 26.0);
	}

	if (test_shadow_map(shadow_map, shadow_coords + vec3(2.0 * offset.x, 0.0, 0.0), epsilon)){
		occlusion -= (1.0 / 26.0);
	}*/
	
	return occlusion;
}

float get_shadow_occlusion(sampler2D shadow_map_low, vec3 shadow_coords_low, 
							sampler2D shadow_map_high, vec3 shadow_coords_high, 
							float light_distance, float epsilon){

	//if (light_distance > LIGHT_HIGH_DISTANCE){
		return get_shadow_value(shadow_map_low, shadow_coords_low, SHADOW_EPSILON_LOW);
	//} else{
		//return get_shadow_value(shadow_map_high, shadow_coords_high, SHADOW_EPSILON_HIGH);

//	}
	
	
	
	

	return 1.0;
}