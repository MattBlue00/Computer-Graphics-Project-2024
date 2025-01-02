// PhongShader.frag

// DIFFUSION MODEL: LAMBERT
// REFLECTION MODEL: PHONG

#version 450

const int LIGHTS_COUNT = 11;

// LAYOUT BINDINGS AND LOCATIONS

layout(binding = 0) uniform PhongUniformBufferObject
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
    float cosIn;
    float cosOut;
} gubo;

layout(location = 0) in vec3 fragPos;
layout(location = 1) in vec3 fragNorm;
layout(location = 2) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

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

    if (distance > 1.0 || (i >= 6 && distance > 0.1)) {
        attenuation = 0.0;
    }
    
    return lightColor * lightIntensity * attenuation;
}

// SPOT LIGHT DIRECTION

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
    float cosTheta = dot(normalize(lightDirection), -normalize(directionVector));
    float falloff = clamp((cosTheta - gubo.cosOut) / (gubo.cosIn - gubo.cosOut), 0.0f, 1.0f);
    return lightColor * pow(lightIntensity / dist, attenuationFactor) * falloff;
}

// BRDF

vec3 BRDF(vec3 Albedo, vec3 Norm, vec3 EyeDir, vec3 LD) {
    vec3 Diffuse = Albedo * max(dot(Norm, LD), 0.0f);   // lambert diffuse model
    vec3 Specular = vec3(pow(max(dot(EyeDir, -reflect(LD, Norm)), 0.0f), 160.0f));  // phong specular model
    return Diffuse + Specular;
}

// MAIN

void main()
{
    vec3 Norm = normalize(fragNorm);
    vec3 EyeDir = normalize(gubo.eyePos - fragPos);
    vec3 Albedo = texture(texSampler, fragTexCoord).xyz;
    
    vec3 ambientLightDirection = gubo.ambientLightDir;
    vec4 ambientLightColor = gubo.ambientLightColor;
    
    vec3 LD;    // light direction
    vec3 LC;    // light color

    vec3 RendEqSol = vec3(0);
    
    LD = point_light_dir(fragPos, 0);
    LC = point_light_color(fragPos, 0);
    RendEqSol += BRDF(Albedo, Norm, EyeDir, LD) * LC * gubo.lightOn[0];
    
    LD = point_light_dir(fragPos, 1);
    LC = point_light_color(fragPos, 1);
    RendEqSol += BRDF(Albedo, Norm, EyeDir, LD) * LC * gubo.lightOn[1];
    
    LD = point_light_dir(fragPos, 2);
    LC = point_light_color(fragPos, 2);
    RendEqSol += BRDF(Albedo, Norm, EyeDir, LD) * LC * gubo.lightOn[2];
    
    LD = point_light_dir(fragPos, 3);
    LC = point_light_color(fragPos, 3);
    RendEqSol += BRDF(Albedo, Norm, EyeDir, LD) * LC * gubo.lightOn[3];
    
    LD = point_light_dir(fragPos, 4);
    LC = point_light_color(fragPos, 4);
    RendEqSol += BRDF(Albedo, Norm, EyeDir, LD) * LC * gubo.lightOn[4];
    
    LD = point_light_dir(fragPos, 5);
    LC = point_light_color(fragPos, 5);
    RendEqSol += BRDF(Albedo, Norm, EyeDir, LD) * LC * gubo.lightOn[5];
    
    LD = point_light_dir(fragPos, 6);
    LC = point_light_color(fragPos, 6);
    RendEqSol += BRDF(Albedo, Norm, EyeDir, LD) * LC * gubo.lightOn[6];
    
    LD = point_light_dir(fragPos, 7);
    LC = point_light_color(fragPos, 7);
    RendEqSol += BRDF(Albedo, Norm, EyeDir, LD) * LC * gubo.lightOn[7];
    
    LD = spot_light_dir(fragPos, 8);
    LC = spot_light_color(fragPos, 8);
    RendEqSol += BRDF(Albedo, Norm, EyeDir, LD) * LC * gubo.lightOn[8];
    
    LD = spot_light_dir(fragPos, 9);
    LC = spot_light_color(fragPos, 9);
    RendEqSol += BRDF(Albedo, Norm, EyeDir, LD) * LC * gubo.lightOn[9];
    
    LD = spot_light_dir(fragPos, 10);
    LC = spot_light_color(fragPos, 10);
    RendEqSol += BRDF(Albedo, Norm, EyeDir, LD) * LC * gubo.lightOn[10];
    
    float reductionFactor = 0.9f;
    
    vec3 ambientDiffuse = Albedo * (max(dot(Norm, ambientLightDirection), 0.0) * 0.9 + 0.1);
    ambientDiffuse *= reductionFactor;
    vec3 ambientSpecular = vec3(pow(max(dot(EyeDir, -reflect(ambientLightDirection, Norm)), 0.0), 64.0));
    ambientSpecular *= reductionFactor;
    
    vec3 ambientCorrection =
        mix(vec3(0.18, 0.12, 0.08), vec3(0.2, 0.1, 0.1), bvec3(Norm.x > 0.0)) * (Norm.x * Norm.x) +
        mix(vec3(0.1), vec3(0.06, 0.2, 0.2), bvec3(Norm.y > 0.0)) * (Norm.y * Norm.y) +
        mix(vec3(0.06, 0.12, 0.14), vec3(0.16, 0.04, 0.08), bvec3(Norm.z > 0.0)) * (Norm.z * Norm.z);
    ambientCorrection *= Albedo;
    ambientCorrection *= reductionFactor;
    
    outColor = vec4(((ambientDiffuse + ambientSpecular * (1.0 - gubo.eyeDir.w)) * ambientLightColor.xyz) + ambientCorrection, 1.0) + vec4(RendEqSol, 1.0);
}
