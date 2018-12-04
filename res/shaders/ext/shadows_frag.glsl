uniform sampler2D shadow_maps[SHADOW_CASCADES];
uniform float shadow_cascade_end[SHADOW_CASCADES];
uniform vec2 projection_depth;
in vec4 shadow_coords[SHADOW_CASCADES];
in float clip_space_z;
in vec4 view_space_pos;

float linearize(float depth){
	float near = projection_depth.x;
	float far = projection_depth.y;

	return (2 * near) / (far + near - depth * (far - near));
}

vec3 project_shadow_coords(vec4 coords){
	return (coords.xyz / coords.w) / 2.0 + 0.5; 
}

bool test_shadow_map(sampler2D shadow_map, vec3 coords, float epsilon){
	float depth = linearize(texture(shadow_map, coords.xy).r);
	float depth_world = linearize(coords.z);

	return depth + epsilon < depth_world;
}

float get_shadow_value(sampler2D shadow_map, vec3 coords, float epsilon){
	
	float occlusion = 0.0;

	vec2 offset = vec2(1.0 / (1.5 * 2048.0));
	

	for (int y = -1; y <= 1; y++){
		for (int x = -1; x <= 1; x++){
			vec3 offset = vec3(x * offset.x, y * offset.y, 0.0);
			if (test_shadow_map(shadow_map, coords + offset, epsilon)){
				occlusion += 1.0;
			}
		}
	}

	
	return 1.0 - (occlusion / (3.0 * 3.0)) * 0.7;
}

float get_shadow_occlusion(){
	
	float depth = view_space_pos.z / projection_depth.y;

	for (int i = 0; i < SHADOW_CASCADES; i++){
			if (depth <= shadow_cascade_end[i] + 0.001){
				vec3 coords = project_shadow_coords(shadow_coords[i]);
				return get_shadow_value(shadow_maps[i], coords, 0.0000005);
			}
	}

	return 1.0;
}