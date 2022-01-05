#version 330

// Input
// Get values from vertex shader
in vec3 frag_position;
in vec3 frag_normal;
in vec2 frag_texture;
in vec3 frag_color;

// Output
layout(location = 0) out vec4 out_color;
layout(location = 1) out vec4 out_normal;
layout(location = 2) out vec3 out_texture;


void main()
{
    // Write pixel out color
    out_color = vec4(0, 0, 0.5, 1);
    out_normal = vec4(frag_normal, 1);
    out_texture = vec3(frag_texture, 1);

}
