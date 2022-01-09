#version 330

// Input
in vec3 world_position;
in vec3 world_normal;

// Uniforms for light properties
uniform vec3 light_position1;
uniform vec3 light_position2;
uniform vec3 light_position3;
uniform vec3 light_position4;
uniform vec3 light_position5;
uniform vec3 light_position6;
uniform vec3 light_position7;
uniform vec3 light_position8;
uniform vec3 light_position9;

uniform vec3 eye_position;
uniform float material_kd;
uniform float material_ks;
uniform int material_shininess;

uniform float spot_mode;
uniform float floor_mode;
uniform float disco_mode;

uniform vec3 object1_color;
uniform vec3 object2_color;
uniform vec3 object3_color;
uniform vec3 object4_color;
uniform vec3 object5_color;
uniform vec3 object6_color;
uniform vec3 object7_color;
uniform vec3 object8_color;
uniform vec3 object9_color;

uniform vec3 spotLightPos[4];
uniform vec3 spotLightCol[4];
uniform vec3 spotLightDir[4];

// Output
layout(location = 0) out vec4 out_color;

vec3 PointLightContribution(vec3 lightPos, vec3 lightColor);
vec3 SpotLightContribution(vec3 lightPos, vec3 lightColor, vec3 light_direction);

void main()
{
    vec3 color;
    color = vec3(0);

    if (floor_mode == 1) {
        color = color + PointLightContribution(light_position1, object1_color);
        color = color + PointLightContribution(light_position2, object2_color);
        color = color + PointLightContribution(light_position3, object3_color);
        color = color + PointLightContribution(light_position4, object4_color);
        color = color + PointLightContribution(light_position5, object5_color);
        color = color + PointLightContribution(light_position6, object6_color);
        color = color + PointLightContribution(light_position7, object7_color);
        color = color + PointLightContribution(light_position8, object8_color);
        color = color + PointLightContribution(light_position9, object9_color);
    }

    if (spot_mode == 1) {
        for (int i = 0; i < 4; i++) {
            color = color + SpotLightContribution(spotLightPos[i], spotLightCol[i], spotLightDir[i]);
        }
    }

    out_color = vec4(color, 1.f);
}

vec3 PointLightContribution(vec3 lightPos, vec3 lightColor)
{
    vec3 col;
    col = vec3(0);

    if (lightColor.r >= 0 || lightColor.g >= 0 || lightColor.b >= 0) {
        vec3 N = normalize(world_normal);
        vec3 L = normalize(lightPos - world_position);
        vec3 R = normalize(reflect(L, world_normal));
        vec3 V = normalize(eye_position - world_position);
        vec3 H = normalize(L + V);

        // Define ambient, diffuse and specular light components
        float ambient_light = 0;
        float diffuse_light = material_kd * max(dot(N, L), 0);

        float specular_light = 0;
        if (diffuse_light > 0)
        {
            specular_light = material_ks * pow(max(dot(N, H), 0), material_shininess);
        }

        float intensity = 0;
        float d = distance(lightPos, world_position);

        // float attenuation_factor = 1.f / max(d * d, 1.f);
        float attenuation_factor = 0;
        float light_radius = 1.5;
        if (d <= light_radius) {
            attenuation_factor = pow(light_radius - d, 2);
        }

        intensity = ambient_light + attenuation_factor * (diffuse_light + specular_light);

        // Write pixel out color
        col = lightColor * intensity;
    }
    return col;
}

vec3 SpotLightContribution(vec3 lightPos, vec3 lightColor, vec3 light_direction)
{
    vec3 col;
    col = vec3(0);

    if (lightColor.r >= 0 || lightColor.g >= 0 || lightColor.b >= 0) {
        vec3 N = normalize(world_normal);
        vec3 L = normalize(lightPos - world_position);
        vec3 R = normalize(reflect(L, world_normal));
        vec3 V = normalize(eye_position - world_position);
        vec3 H = normalize(L + V);

        // Define ambient, diffuse and specular light components
        float ambient_light = 0;
        float diffuse_light = material_kd * max(dot(N, L), 0);

        float specular_light = 0;
        if (diffuse_light > 0)
        {
            specular_light = material_ks * pow(max(dot(N, H), 0), material_shininess);
        }

        float intensity = 0;
        float attenuation_factor = 0;
        
        float cutoff_angle = radians(15);
        // vec3 light_direction = vec3(0, -1, 0);
        float spot_light = dot(-L, light_direction);
        float spot_light_limit = cos(cutoff_angle);

        if (spot_light > spot_light_limit) {
            float linear_att = (spot_light - spot_light_limit) / (1 - spot_light_limit);
            attenuation_factor = pow(linear_att, 2);
        }

        intensity = ambient_light + attenuation_factor * (diffuse_light + specular_light);

        // Write pixel out color
        col = lightColor * intensity;
    }
    return col;
}


