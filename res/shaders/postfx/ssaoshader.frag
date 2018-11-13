in vec2 tex_coords_fs;

uniform sampler2D tex_unit0; //g_position
uniform sampler2D tex_unit1; //g_normal
uniform sampler2D tex_unit2; //noise

uniform vec3 kernel[64];
uniform vec2 resolution;

uniform mat4 projection_matrix;

const float radius = 0.5;
const float bias = 0.025;

out float color;

void main(void){
	
	const vec2 noise_scale = vec2(resolution.x / 4.0, resolution.y / 4.0);
	
	vec3 frag_pos = texture(tex_unit0, tex_coords_fs).xyz;
	vec3 normal = texture(tex_unit1, tex_coords_fs).xyz;
	vec3 random = normalize(texture(tex_unit2, tex_coords_fs * noise_scale).xyz);

	vec3 tangent = normalize(random - normal * dot(random, normal));
	vec3 bitangent = cross(normal, tangent);

	mat3 TBN = mat3(tangent, bitangent, normal);

	float occlusion = 0.0;

	for (int i = 0; i < 64; i++){
		vec3 s = TBN * kernel[i];

		s = frag_pos + s * radius;

		vec4 offset = vec4(s, 1.0);
		offset = projection_matrix * offset;

		offset.xyz /= offset.w;
		offset.xyz = offset.xyz / 2.0 + 0.5;

		float sample_depth = texture(tex_unit0, offset.xy).z;

		float range_check = smoothstep(0.0, 1.0, radius / abs(frag_pos.z - sample_depth));
		occlusion += (sample_depth >= s.z + bias ? 1.0 : 0.0) * range_check;
	}

	occlusion = 1.0 - (occlusion / 64.0);

	color = occlusion;
}