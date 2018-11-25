in vec2 tex_coords_fs;

uniform sampler2D tex_unit; //depth

uniform vec3 kernel[128];

uniform vec2 resolution;
uniform vec2 projection_depth;

const float radius = 0.3;
const float falloff = 0.2;
const float intensity = 1.0;
const float base = 0.0;
const float area = 1.1;


out vec4 color;

float get_depth(vec2 coords){
	float depth = texture(tex_unit, coords).r;

	float near = projection_depth.x;
	float far = projection_depth.y;
	
	return 2.0 * near * far / (far + near - (2.0 * depth - 1.0) * (far - near));
}

vec3 get_normal(float depth, vec2 tex_coords) {
  
  vec2 texel_size = 1.0/textureSize(tex_unit, 0);

  vec2 offset1 = vec2(texel_size.x, 0.0);
  vec2 offset2 = vec2(0.0, texel_size.y);
  
  float depth1 = get_depth(tex_coords + offset1);
  float depth2 = get_depth(tex_coords + offset2);
  
  vec3 p1 = vec3(offset1, depth1 - depth);
  vec3 p2 = vec3(offset2, depth2 - depth);
  
  vec3 normal = cross(p1, p2);
  normal.z = -normal.z;
  
  return normalize(normal);
}

vec3 reflection(vec3 v1,vec3 v2)
{
    vec3 result = 2.0 * dot(v2, v1) * v2;
    result = v1 - result;
    return result;
}

void main(void){
	
	float depth = get_depth(tex_coords_fs);
	
	vec3 position = vec3(tex_coords_fs, depth);

	vec3 normal = get_normal(depth, tex_coords_fs);
	float depth_radius = radius / depth;

	float occlusion = 0.0;

	const int KERNEL_SIZE = 128;

   	for (int i = 0; i < KERNEL_SIZE; i++){
		vec3 ray = depth_radius * kernel[i];

		vec3 hemi_ray = position + sign(dot(ray, normal)) * ray;

		float occlusion_depth = get_depth(clamp(hemi_ray.xy, 0.0, 1.0));
		float difference = depth - occlusion_depth;

		float range_check = smoothstep(0.0, 1.0, radius / abs(position.z - occlusion_depth));

		occlusion += (step(falloff, difference) * (1.0 - smoothstep(falloff, area, difference))) * range_check;	
	}

	float ao = 1.0 - intensity * occlusion * (1.0 / KERNEL_SIZE);
	ao = clamp(ao + base,0.0,1.0);

	color = vec4(ao, ao, ao, 1.0);
}