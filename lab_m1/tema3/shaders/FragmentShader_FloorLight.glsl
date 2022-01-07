#version 330

// Input
in vec2 texcoord;

// Uniform properties
uniform sampler2D texture_1;
uniform sampler2D texture_2;

// TODO(student): Declare various other uniforms
uniform bool mix_tex;

// Output
layout(location = 0) out vec4 out_color;


void main()
{
    // TODO(student): Calculate the out_color using the texture2D() function.
    vec4 c1 = texture2D(texture_1, texcoord);
    vec4 c2 = texture2D(texture_2, texcoord);
    
    if (mix_tex == true) {
        out_color = mix(c1, c2, 0.5);
    } else {
        out_color = c1;
    }

    if (out_color.a < 0.5) {
        discard;
    }
}
