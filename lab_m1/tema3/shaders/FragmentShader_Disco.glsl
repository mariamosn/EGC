#version 330

// Input
in vec2 texcoord;

// Uniform properties
uniform sampler2D texture_1;

// Output
layout(location = 0) out vec4 out_color;


void main()
{
    // Calculate the out_color using the texture2D() function.
    vec4 c1 = texture2D(texture_1, texcoord);
    out_color = c1;

    if (out_color.a < 0.5) {
        discard;
    }
}
