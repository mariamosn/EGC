#version 330

// Input
in vec3 world_position;
in vec3 world_normal;

// Uniforms for light properties
uniform vec3 spotLightPos[4];
uniform vec3 spotLightCol[4];

uniform vec3 eye_position;
uniform float material_kd;
uniform float material_ks;
uniform int material_shininess;

uniform float spot_mode;
uniform float floor_mode;
uniform float disco_mode;

uniform vec3 object_color;

// Output
layout(location = 0) out vec4 out_color;

void main()
{
    out_color = vec4(object_color, 0.5);
}