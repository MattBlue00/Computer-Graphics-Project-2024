#version 450

const int LIGHTS_COUNT = 2;

// LAYOUT BINDINGS AND LOCATIONS

layout(binding = 0) uniform UniformBufferObject
{
    mat4 mvpMat;
    mat4 mMat;
    mat4 nMat;
} ubo;

layout(binding = 1) uniform sampler2D texSampler;

/*
layout(binding = 2) uniform GlobalUniformBufferObject
{
    vec3 lightDir;
    vec4 lightColor;
    vec3 eyePos;
    vec4 eyeDir;
} gubo;*/

layout(binding = 2) uniform GlobalUniformBufferObject {
    vec3 lightDir[LIGHTS_COUNT];
    vec3 lightPos[LIGHTS_COUNT];
    vec4 lightColor[LIGHTS_COUNT];
    vec3 eyePos;
    vec4 eyeDir;
    vec4 lightOn;
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
    float attenuationFactor = 2.0f;
    vec3 directionVector = lightPosition - pos;
    float dist = length(directionVector);
    return lightColor * pow(lightIntensity / dist, attenuationFactor);
}

// SPOT LIGHT DIRECTION
/*
vec3 spot_light_dir(vec3 pos, int i) {
    vec3 lightPosition = gubo.lightPos[i];
    vec3 directionVector = lightPosition - pos;
    return normalize(directionVector);
}

// SPOT LIGHT COLOR

vec3 spot_light_color(vec3 pos, int i) {
    vec3 lightPosition = gubo.lightPos[i];
    float lightIntensity = gubo.lightColor[i].a;
    vec3 lightColor = gubo.lightColor[i].rgb;
    vec3 lightDirection = gubo.lightDir[i];
    vec3 directionVector = lightPosition - pos;
    float dist = length(directionVector);
    float attenuationFactor = 2.0f;
    float cosTheta = dot(normalize(lightDirection), directionVector);
    float falloff = clamp((cosTheta - gubo.cosOut) / (gubo.cosIn - gubo.cosOut), 0.0f, 1.0f);
    return lightColor * pow(lightIntensity / dist, attenuationFactor) * falloff;
}
*/
// BRDF

vec3 BRDF(vec3 Albedo, vec3 Norm, vec3 EyeDir, vec3 LD) {
    vec3 Diffuse;
    vec3 Specular;
    Diffuse = Albedo * max(dot(Norm, LD),0.0f);
    Specular = vec3(pow(max(dot(EyeDir, -reflect(LD, Norm)),0.0f), 160.0f));
    return Diffuse + Specular;
}

// MAIN

void main()
{
    vec3 Norm = normalize(fragNorm);
    vec3 EyeDir = normalize(gubo.eyePos - fragPos);
    vec3 Albedo = texture(texSampler, fragTexCoord).xyz;
    
    vec4 EyeDirection = gubo.eyeDir;
    vec3 LightDirection = gubo.lightDir[0];
    vec4 LightColor = gubo.lightColor[0];
    
    /*
    vec3 LD;    // light direction
    vec3 LC;    // light color

    vec3 RendEqSol = vec3(0);
    
    for(int i = 1; i < LIGHTS_COUNT; i++){
        LD = point_light_dir(fragPos, i);
        LC = point_light_color(fragPos, i);
        RendEqSol += BRDF(Albedo, Norm, EyeDir, LD) * LC         * gubo.lightOn.x;
    }*/
    
    vec3 Diffuse = Albedo * ((max(dot(Norm, LightDirection), 0.0) * 0.89999997615814208984375) + 0.100000001490116119384765625);
    vec3 Specular = vec3(pow(max(dot(EyeDir, -reflect(LightDirection, Norm)), 0.0), 64.0));
    vec3 Ambient = (((mix(vec3(0.180000007152557373046875, 0.119999997317790985107421875, 0.07999999821186065673828125), vec3(0.20000000298023223876953125, 0.100000001490116119384765625, 0.100000001490116119384765625), bvec3(Norm.x > 0.0)) * (Norm.x * Norm.x)) + (mix(vec3(0.100000001490116119384765625), vec3(0.0599999986588954925537109375, 0.20000000298023223876953125, 0.20000000298023223876953125), bvec3(Norm.y > 0.0)) * (Norm.y * Norm.y))) + (mix(vec3(0.0599999986588954925537109375, 0.119999997317790985107421875, 0.14000000059604644775390625), vec3(0.1599999964237213134765625, 0.039999999105930328369140625, 0.07999999821186065673828125), bvec3(Norm.z > 0.0)) * (Norm.z * Norm.z))) * Albedo;
    outColor = vec4(((Diffuse + (Specular * (1.0 - EyeDirection.w))) * LightColor.xyz) + Ambient, 1.0) + vec4(RendEqSol, 1.0);
}

