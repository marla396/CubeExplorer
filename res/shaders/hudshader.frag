in vec2 pass_tex_coords;
out vec4 color;

uniform sampler2D tex_unit;

void main(void){
	float depth = texture(tex_unit, pass_tex_coords).r;

	const float near = 0.1;
	const float far = 1024.0;



	depth = 2.0 * near * far / (far + near - (2.0 * depth - 1.0) * (far - near));

	depth /= 1000.0;

	color = vec4(depth, depth, depth, 1.0);
}