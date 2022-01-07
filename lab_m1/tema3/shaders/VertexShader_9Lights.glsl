#version 330

// Input
layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texture_coord;

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

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

uniform vec3 object1_color;
uniform vec3 object2_color;
uniform vec3 object3_color;
uniform vec3 object4_color;
uniform vec3 object5_color;
uniform vec3 object6_color;
uniform vec3 object7_color;
uniform vec3 object8_color;
uniform vec3 object9_color;

// Output value to fragment shader
out vec3 color;

vec3 PointLightContribution(vec3 lightPos, vec3 lightColor);

void main()
{
    color = vec3(0);
    color = color + PointLightContribution(light_position1, object1_color);
    color = color + PointLightContribution(light_position2, object2_color);
    color = color + PointLightContribution(light_position3, object3_color);
    color = color + PointLightContribution(light_position4, object4_color);
    color = color + PointLightContribution(light_position5, object5_color);
    color = color + PointLightContribution(light_position6, object6_color);
    color = color + PointLightContribution(light_position7, object7_color);
    color = color + PointLightContribution(light_position8, object8_color);
    color = color + PointLightContribution(light_position9, object9_color);

    gl_Position = Projection * View * Model * vec4(v_position, 1.0);
}

vec3 PointLightContribution(vec3 lightPos, vec3 lightColor) {
    vec3 col;
    col = vec3(0);

    if (lightColor.r > -1 || lightColor.g > -1 || lightColor.b > -1) {
        // TODO(student): Compute world space vectors
        vec3 world_position = (Model * vec4(v_position, 1)).xyz;
        vec3 world_normal = normalize(vec3(Model * vec4(v_normal, 0)));

        vec3 N = world_normal;
        vec3 L = normalize(lightPos - world_position);
        vec3 R = normalize(reflect(-L, N));
        vec3 V = normalize(eye_position - world_position);
        vec3 H = normalize(L + V);

        // TODO(student): Define ambient light component
        float ambient_light = 0;
        // float ambient_light = material_kd * 0.75;

        // TODO(student): Compute diffuse light component
        // float diffuse_light = 0;
        float diffuse_light = material_kd * max(dot(N, L), 0);

        // TODO(student): Compute specular light component
        float specular_light = 0;

        // It's important to distinguish between "reflection model" and
        // "shading method". In this shader, we are experimenting with the Phong
        // (1975) and Blinn-Phong (1977) reflection models, and we are using the
        // Gouraud (1971) shading method. There is also the Phong (1975) shading
        // method, which we'll use in the future. Don't mix them up!
        if (diffuse_light > 0)
        {
            // specular_light = material_ks * pow(max(dot(V, R), 0), material_shininess);
        
            // sau
            specular_light = material_ks * 0.75 * pow(max(dot(N, H), 0), material_shininess);
        }

        // TODO(student): Compute light
        float d = distance(lightPos, v_position);


        // float attenuation_factor = 1 / ((d + 1) * (d + 1));
        float attenuation_factor	= 1 / (2 + 0.10 * d + 0.05 * d * d);
        // float attenuation_factor = 0;
        
        // light_radius reprezinta distanta maxima pana la care ajunge lumina unei surse
        // dist reprezinta distanta de la sursa de lumina la punctul pentru care se calculeaza iluminarea
        
        // chiar daca s-au luat cele mai apropiate lumini fata de obiect,
        // la nivel de pixel, distanta poate fi mai mare decat raza
        float light_radius = 0.5;
        if (d < light_radius) {
            attenuation_factor = pow(light_radius - d, 2);
        }


        float intensity = ambient_light + attenuation_factor * (diffuse_light + specular_light);

        // TODO(student): Send color light output to fragment shader
        // color = vec3(1);
        col = lightColor * intensity;
    }

    return col;
}