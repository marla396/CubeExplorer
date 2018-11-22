in vec2 tex_coords_fs;

uniform sampler2D tex_unit0; //color
uniform sampler2D tex_unit1; //depth
uniform sampler2D tex_unit2; //noise_scale

uniform vec2 resolution;

const float radius = 3.0;
const float falloff = 1.0;
const float intensity = 1.0;
const float base = 1.0;
const float area = 1.0;


out vec4 color;

vec3 get_normal(float depth, vec2 tex_coords) {
  
  vec2 offset1 = vec2(0.0, 0.001);
  vec2 offset2 = vec2(0.001, 0.0);
  
  float depth1 = texture(tex_unit1, tex_coords + offset1).r;
  float depth2 = texture(tex_unit1, tex_coords + offset2).r;
  
  
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
	
	const vec2 noise_scale = vec2(resolution.x / 4.0, resolution.y / 4.0);
	
	vec3 random = normalize(texture(tex_unit2, tex_coords_fs * noise_scale).xyz);
	float depth = texture(tex_unit1, tex_coords_fs).r;
	vec3 position = vec3(tex_coords_fs, depth);

	vec3 normal = get_normal(depth, tex_coords_fs);
	float depth_radius = radius / depth;

	float occlusion = 0.0;

	 vec3 sample_sphere[16] = vec3[]( vec3( 0.5381, 0.1856,-0.4319), vec3( 0.1379, 0.2486, 0.4430),
								  vec3( 0.3371, 0.5679,-0.0057), vec3(-0.6999,-0.0451,-0.0019),
								  vec3( 0.0689,-0.1598,-0.8547), vec3( 0.0560, 0.0069,-0.1843),
								  vec3(-0.0146, 0.1402, 0.0762), vec3( 0.0100,-0.1924,-0.0344),
								  vec3(-0.3577,-0.5301,-0.4358), vec3(-0.3169, 0.1063, 0.0158),
								  vec3( 0.0103,-0.5869, 0.0046), vec3(-0.0897,-0.4940, 0.3287),
								  vec3( 0.7119,-0.0154,-0.0918), vec3(-0.0533, 0.0596,-0.5411),
								  vec3( 0.0352,-0.0631, 0.5460), vec3(-0.4776, 0.2847,-0.0271)
								);

	const int iterations = 16;

   	for (int j = 0; j < iterations; j++){
		vec3 ray = depth_radius * reflection(sample_sphere[j], random);
		vec3 hemi_ray = position + sign(dot(ray,normal)) * ray;

		float occlusion_depth = texture(tex_unit1, clamp(hemi_ray.xy,0.0,1.0)).r;
		float difference = depth - occlusion_depth;

		occlusion += step(falloff, difference) * (1.0-smoothstep(falloff, area, difference));	
	}

	float ao = 1.0 - intensity * occlusion * (1.0 / iterations);
  	float final = clamp(ao + base,0.0,1.0);

	color = vec4(final,final,final,1.0); 
}