// CookTorranceShader.frag

// DIFFUSION MODEL: LAMBERT
// REFLECTION MODEL: COOK-TORRANCE

#version 450

const int LIGHTS_COUNT = 14;

// LAYOUT BINDINGS AND LOCATIONS

layout(binding = 0) uniform CookTorranceUniformBufferObject {
    mat4 mvpMat; // Model-View-Projection matrix
    mat4 mMat;   // Model matrix
    mat4 nMat;   // Normal matrix (transpose(inverse(modelMatrix)))
    float metalness;
    float roughness;
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

// FRESNEL TERM

vec3 fresnel(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

// GEOMETRIC ATTENUATION (MICROFACET)

float geometry_microfacet(float NdotH, float NdotV, float NdotL, float VdotH) {
    float G1 = (2 * NdotH * NdotV) / VdotH;
    float G2 = (2 * NdotH * NdotL) / VdotH;

    return min(1, min(G1, G2));
}

// DISTRIBUTION (GGX)

float distribution_ggx(float NdotH, float roughness) {
    float alpha = roughness * roughness;
    float alpha2 = alpha * alpha;
    float denom = clamp((NdotH * NdotH) * (alpha2 - 1.0) + 1.0, 0.0f, 1.0f); // GGX version
    return alpha2 / (3.141592 * denom * denom);
}

// COOK-TORRANCE SPECULAR REFLECTION

vec3 cook_torrance_specular(vec3 N, vec3 L, vec3 V, vec3 Albedo, float roughness, float metalness) {
    vec3 H = normalize(L + V);

    float NdotL = clamp(dot(N, L), 0.0f, 1.0f);
    float NdotV = clamp(dot(N, V), 0.0f, 1.0f);
    float NdotH = clamp(dot(N, H), 0.0f, 1.0f);
    float VdotH = clamp(dot(V, H), 0.0f, 1.0f);

    vec3 F0 = mix(vec3(0.02), Albedo, metalness);
    vec3 F = fresnel(VdotH, F0);
    float G = geometry_microfacet(NdotH, NdotV, NdotL, VdotH);
    float D = distribution_ggx(NdotH, roughness);

    vec3 specular = (F * G * D) / (4.0 * NdotL * NdotV + 0.001); // Avoid division by zero
    return specular;
}

// BRDF METALLIC WITH COOK-TORRANCE AND LAMBERT

vec3 BRDF(vec3 Albedo, vec3 Norm, vec3 EyeDir, vec3 LD, float roughness, float metalness) {
    // Lambert Diffuse
    vec3 Diffuse = Albedo * (1.0 - metalness) * max(dot(Norm, LD), 0.0);

    // Cook-Torrance Specular
    vec3 Specular = Albedo * metalness * cook_torrance_specular(Norm, LD, EyeDir, Albedo, roughness, metalness);

    return Diffuse + Specular;
}

// MAIN FUNCTION

void main()
{
    vec3 Norm = normalize(fragNorm); // Normal vector
    vec3 EyeDir = normalize(gubo.eyePos - fragPos); // View vector
    vec3 Albedo = texture(texSampler, fragTexCoord).rgb; // Albedo color from texture

    vec3 LD;    // light direction
    vec3 LC;    // light color

    vec3 RendEqSol = vec3(0);
    
    LD = point_light_dir(fragPos, 0);
    LC = point_light_color(fragPos, 0);
    RendEqSol += BRDF(Albedo, Norm, EyeDir, LD, ubo.roughness, ubo.metalness) * LC * gubo.lightOn[0];
    
    LD = point_light_dir(fragPos, 1);
    LC = point_light_color(fragPos, 1);
    RendEqSol += BRDF(Albedo, Norm, EyeDir, LD, ubo.roughness, ubo.metalness) * LC * gubo.lightOn[1];
    
    LD = point_light_dir(fragPos, 2);
    LC = point_light_color(fragPos, 2);
    RendEqSol += BRDF(Albedo, Norm, EyeDir, LD, ubo.roughness, ubo.metalness) * LC * gubo.lightOn[2];
    
    LD = point_light_dir(fragPos, 3);
    LC = point_light_color(fragPos, 3);
    RendEqSol += BRDF(Albedo, Norm, EyeDir, LD, ubo.roughness, ubo.metalness) * LC * gubo.lightOn[3];
    
    LD = point_light_dir(fragPos, 4);
    LC = point_light_color(fragPos, 4);
    RendEqSol += BRDF(Albedo, Norm, EyeDir, LD, ubo.roughness, ubo.metalness) * LC * gubo.lightOn[4];
    
    LD = point_light_dir(fragPos, 5);
    LC = point_light_color(fragPos, 5);
    RendEqSol += BRDF(Albedo, Norm, EyeDir, LD, ubo.roughness, ubo.metalness) * LC * gubo.lightOn[5];
    
    LD = point_light_dir(fragPos, 6);
    LC = point_light_color(fragPos, 6);
    RendEqSol += BRDF(Albedo, Norm, EyeDir, LD, ubo.roughness, ubo.metalness) * LC * gubo.lightOn[6];
    
    LD = point_light_dir(fragPos, 7);
    LC = point_light_color(fragPos, 7);
    RendEqSol += BRDF(Albedo, Norm, EyeDir, LD, ubo.roughness, ubo.metalness) * LC * gubo.lightOn[7];
    
    LD = spot_light_dir(fragPos, 8);
    LC = spot_light_color(fragPos, 8);
    RendEqSol += BRDF(Albedo, Norm, EyeDir, LD, ubo.roughness, ubo.metalness) * LC * gubo.lightOn[8];
    
    LD = spot_light_dir(fragPos, 9);
    LC = spot_light_color(fragPos, 9);
    RendEqSol += BRDF(Albedo, Norm, EyeDir, LD, ubo.roughness, ubo.metalness) * LC * gubo.lightOn[9];
    
    LD = spot_light_dir(fragPos, 10);
    LC = spot_light_color(fragPos, 10);
    RendEqSol += BRDF(Albedo, Norm, EyeDir, LD, ubo.roughness, ubo.metalness) * LC * gubo.lightOn[10];
    
    LD = spot_light_dir(fragPos, 11);
    LC = spot_light_color(fragPos, 11);
    RendEqSol += BRDF(Albedo, Norm, EyeDir, LD, ubo.roughness, ubo.metalness) * LC * gubo.lightOn[11];
    
    LD = spot_light_dir(fragPos, 12);
    LC = spot_light_color(fragPos, 12);
    RendEqSol += BRDF(Albedo, Norm, EyeDir, LD, ubo.roughness, ubo.metalness) * LC * gubo.lightOn[12];
    
    LD = spot_light_dir(fragPos, 13);
    LC = spot_light_color(fragPos, 13);
    RendEqSol += BRDF(Albedo, Norm, EyeDir, LD, ubo.roughness, ubo.metalness) * LC * gubo.lightOn[13];
    
    outColor = vec4(RendEqSol, 1.0);
}
