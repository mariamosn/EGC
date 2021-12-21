#version 330

// Input
// Get vertex attributes from each location
layout(location = 0) in vec3 v_position;
layout(location = 2) in vec2 v_texture;
layout(location = 3) in vec3 v_normal;
layout(location = 1) in vec3 v_color;

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform float T;

// Output
// Output values to fragment shader
out vec3 frag_position;
out vec3 frag_normal;
out vec2 frag_texture;
out vec3 frag_color;

void main()
{
    // Send output to fragment shader
    frag_position = v_position;
    frag_normal = v_normal + vec3(sin(T), cos(T), 0);
    frag_texture = v_texture;
    frag_color = v_color;

    // Compute gl_Position
    gl_Position = Projection * View * (Model * vec4(v_position, 1.0) + vec4(0, sin(T), 0, 0));

}
