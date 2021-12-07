#version 330

// Input
// TODO(student): Get vertex attributes from each location
layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texture;
layout(location = 3) in vec3 v_color;
// layout(location = 3) in vec3 v_normal;
// layout(location = 1) in vec3 v_color;

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform float T;
uniform float Health;
uniform float Random;

// Output
// TODO(student): Output values to fragment shader
out vec3 frag_position;
out vec3 frag_normal;
out vec2 frag_texture;
out vec3 frag_color;


void main()
{
    // TODO(student): Send output to fragment shader
    frag_position = v_position;
    if (Health > 5) {
        frag_color = v_normal + vec3(sin(T) / 5, cos(T) / 2, sin(T));
        // frag_normal = v_normal + vec3(sin(T) / 5, cos(T) / 2, sin(T));
    } else {
        frag_color = v_normal + vec3(sin(T), cos(T), 0);
        // frag_normal = v_normal + vec3(sin(T), cos(T), 0);
    }
    frag_texture = v_texture;
    // frag_color = v_color;
    frag_normal = v_normal;

    // TODO(student): Compute gl_Position
    gl_Position = Projection * View * (Model * vec4(v_position, 1.0) + vec4(0, sin(T) / 5, 0, 0));
}