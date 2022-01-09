#version 330

// Input
in vec3 world_position;
in vec3 world_normal;

// Uniforms for light properties
uniform vec3 spotLightPos[4];
uniform vec3 spotLightCol[4];
uniform vec3 spotLightDir[4];

uniform vec3 disco_pos;
uniform sampler2D gen_texture;
uniform float time;

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

vec3 SpotLightContribution(vec3 lightPos, vec3 lightColor, vec3 light_direction);
vec3 DiscoLightContribution();

void main()
{
    vec3 color;
    color = vec3(0);

    if (floor_mode == 1) {
        color = color + object_color * 0.75;
    }

    if (spot_mode == 1) {
        for (int i = 0; i < 4; i++) {
            color = color + SpotLightContribution(spotLightPos[i], spotLightCol[i], spotLightDir[i]);
        }
    }

    if (disco_mode == 1) {
        color = color + DiscoLightContribution();
    }

    out_color = vec4(color, 1.f);
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

vec3 DiscoLightContribution()
{
    vec3 col;
    col = vec3(0);

    vec3 N = normalize(world_normal);
    vec3 L = normalize(disco_pos - world_position);
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
    vec3 light_direction = world_position - disco_pos;

    // texcoord este coordonata de textura utilizata pentru esantionare
    vec2 texcoord;
    texcoord.x = (1.0 / (2 * 3.14159)) * atan (light_direction.x, light_direction.z);
    texcoord.y = (1.0 / 3.14159) * acos (light_direction.y / length (light_direction));

    texcoord = vec2(texcoord.x + time / 10, texcoord.y);

    // lightColor este culoarea corespunzatoare pozitiei world_position
    // vec3 lightColor = texture2D(gen_texture, texcoord).xyz;
    vec3 lightColor = texture(gen_texture, texcoord).xyz;
    // vec3 lightColor = vec3(1);

    float spot_light = dot(-L, light_direction);
    float spot_light_limit = cos(cutoff_angle);

    if (spot_light > spot_light_limit) {
        float linear_att = (spot_light - spot_light_limit) / (1 - spot_light_limit);
        attenuation_factor = pow(linear_att, 2);
    }

    // intensity = ambient_light + attenuation_factor * (diffuse_light + specular_light);
    intensity = 0.5;

    // Write pixel out color
    col = lightColor * intensity;

    return col;
}

