in vec2 tex_coords_fs;

#define KERNEL_SIZE 128

uniform sampler2D tex_unit0; //depth
uniform sampler2D tex_unit1; //noise

uniform vec3 kernel[KERNEL_SIZE];
uniform vec2 projection_depth;
uniform vec2 screen_dimensions;
uniform mat4 projection_matrix;

const float RADIUS = 0.5;

out vec4 color;

float get_depth(vec2 coords){
	float depth = texture(tex_unit0, coords).r;

	float near = projection_depth.x;
	float far = projection_depth.y;
	
	return 2.0 * near * far / (far + near - (2.0 * depth - 1.0) * (far - near));
}

vec3 get_normal(float depth, vec2 tex_coords) {
  

  vec2 offset1 = vec2(0.0, 1.0 / screen_dimensions.y);
  vec2 offset2 = vec2(1.0 / screen_dimensions.x, 0.0);
  
  float depth1 = get_depth(tex_coords + offset1).r;
  float depth2 = get_depth(tex_coords + offset2).r;
  
  vec3 p1 = vec3(offset1, depth1 - depth);
  vec3 p2 = vec3(offset2, depth2 - depth);
  
  vec3 normal = cross(p1, p2);

  return normalize(normal);
}

vec3 get_random(vec2 tex_coords){
	vec2 scale = vec2(screen_dimensions) / 4.0;

	return texture(tex_unit1, tex_coords * scale).xyz;
}


void main(void){

	float depth = get_depth(tex_coords_fs);

	if (depth >= projection_depth.y - 1.0){
		color = vec4(1.0);
		return;
	}

	vec3 frag_pos = vec3(tex_coords_fs, depth);
	vec3 normal = get_normal(depth, tex_coords_fs);

	vec3 randomVec = get_random(tex_coords_fs);
	vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
	vec3 bitangent = cross(normal, tangent);
	mat3 TBN = mat3(tangent, bitangent, normal);

	float occlusion = 0.0;

	for (int i = 0; i < KERNEL_SIZE; i++){	
        vec3 d = TBN * kernel[i];
		d = frag_pos + d * RADIUS;
		
        float sample_depth = get_depth(d.xy);

		float range_check = smoothstep(0.0, 1.0, RADIUS / abs(frag_pos.z - sample_depth));

		occlusion += (sample_depth < d.z ? 1.0 : 0.0) * range_check;
	}

	occlusion = 1.0 - occlusion / KERNEL_SIZE;
	
	color.rgb = vec3(occlusion);
}