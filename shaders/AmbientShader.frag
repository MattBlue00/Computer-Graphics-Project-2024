#version 450

const int LIGHTS_COUNT = 8;

// LAYOUT BINDINGS AND LOCATIONS

layout(binding = 0) uniform UniformBufferObject
{
    mat4 mvpMat;
    mat4 mMat;
    mat4 nMat;
} ubo;

layout(binding = 1) uniform sampler2D texSampler;

layout(binding = 2) uniform GlobalUniformBufferObject {
    vec3 ambientLightDir;
    vec4 ambientLightColor;
    vec3 lightDir[LIGHTS_COUNT];
    vec3 lightPos[LIGHTS_COUNT];
    vec4 lightColor[LIGHTS_COUNT];
    vec3 eyePos;
    vec4 eyeDir;
    vec3 lightOn[LIGHTS_COUNT];
} gubo;

layout(location = 0) in vec3 fragPos;
layout(location = 1) in vec3 fragNorm;
layout(location = 2) in vec2 fragTexCoord;
layout(location = 0) out vec4 outColor;

// DIRECT LIGHT DIRECTION

vec3 direct_light_dir(vec3 pos, int i) {
    return gubo.lightDir[i];
}

// DIRECT LIGHT COLOR

vec3 direct_light_color(vec3 pos, int i) {
    return gubo.lightColor[i].rgb;
}

// POINT LIGHT DIRECTION

vec3 point_light_dir(vec3 pos, int i) {
    vec3 lightPosition = gubo.lightPos[i];
    vec3 directionVector = lightPosition - pos;
    return normalize(directionVector);
}

// POINT LIGHT COLOR

vec3 point_light_color(vec3 pos, int i) {
    vec3 lightPosition = gubo.lightPos[i];
    float lightIntensity = gubo.lightColor[i].a;
    vec3 lightColor = gubo.lightColor[i].rgb;
    
    float distance = length(lightPosition - pos);
    float constant = 1.0;
    float linear = 0.35;
    float quadratic = 0.44;
    
    float attenuation = 1.0 / (constant + linear * distance + quadratic * distance * distance);

    if (distance > 1.0) {
        attenuation = 0.0;
    }
    
    if (i >= 6 && distance > 0.15) {
        attenuation = 0.0;
    }
    
    return lightColor * lightIntensity * attenuation;
}

// BRDF

vec3 BRDF(vec3 Albedo, vec3 Norm, vec3 EyeDir, vec3 LD) {
    vec3 Diffuse = Albedo * max(dot(Norm, LD), 0.0f);
    vec3 Specular = vec3(pow(max(dot(EyeDir, -reflect(LD, Norm)), 0.0f), 160.0f));
    return Diffuse + Specular;
}

// MAIN

void main()
{
    vec3 Norm = normalize(fragNorm);
    vec3 EyeDir = normalize(gubo.eyePos - fragPos);
    vec3 Albedo = texture(texSampler, fragTexCoord).xyz;
    
    vec3 LightDirection = gubo.ambientLightDir;
    vec4 LightColor = gubo.ambientLightColor;
    
    vec3 LD;    // light direction
    vec3 LC;    // light color

    vec3 RendEqSol = vec3(0);
    
    for(int i = 0; i < LIGHTS_COUNT; i++){
        LD = point_light_dir(fragPos, i);
        LC = point_light_color(fragPos, i);
        RendEqSol += BRDF(Albedo, Norm, EyeDir, LD) * LC * gubo.lightOn[i];
    }
    
    vec3 Diffuse = Albedo * (max(dot(Norm, LightDirection), 0.0) * 0.9 + 0.1);
    vec3 Specular = vec3(pow(max(dot(EyeDir, -reflect(LightDirection, Norm)), 0.0), 64.0));
    vec3 Ambient = mix(vec3(0.18, 0.12, 0.08), vec3(0.2, 0.1, 0.1), bvec3(Norm.x > 0.0)) * (Norm.x * Norm.x) +
                   mix(vec3(0.1), vec3(0.06, 0.2, 0.2), bvec3(Norm.y > 0.0)) * (Norm.y * Norm.y) +
                   mix(vec3(0.06, 0.12, 0.14), vec3(0.16, 0.04, 0.08), bvec3(Norm.z > 0.0)) * (Norm.z * Norm.z);
    Ambient *= Albedo;
    
    outColor = vec4(((Diffuse + Specular * (1.0 - gubo.eyeDir.w)) * LightColor.xyz) + Ambient, 1.0) + vec4(RendEqSol, 1.0);
}
