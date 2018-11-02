#define MAX_RADIUS 50

in vec2 pass_tex_coords;
uniform sampler2D tex_unit;

uniform int radius;
uniform float kernel[MAX_RADIUS];
out vec4 color;

void main(void)
{
    float offset = 1.0 / textureSize(tex_unit, 0).x;
    color = vec4(0.0);
    
    color += kernel[0] * texture(tex_unit, pass_tex_coords);
    for (int i = 1; i < radius; i++){
        color += kernel[i] * texture(tex_unit, pass_tex_coords - vec2(offset * i, 0.0));
        color += kernel[i] * texture(tex_unit, pass_tex_coords + vec2(offset * i, 0.0));
    }
}