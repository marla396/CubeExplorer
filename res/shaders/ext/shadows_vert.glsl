out vec4 shadow_coords[SHADOW_CASCADES];
out float clip_space_z;
out vec4 view_space_pos;

uniform mat4 shadow_transforms[SHADOW_CASCADES];
uniform mat4 view_matrix;

void transform_shadow_coords(vec4 world_pos, float clip_z){
	for (int i = 0; i < SHADOW_CASCADES; i++){
		shadow_coords[i] = shadow_transforms[i] * world_pos;
	}

	clip_space_z = clip_z;
	view_space_pos = view_matrix * world_pos;
}