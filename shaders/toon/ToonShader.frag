// ToonShader.frag

#version 450

const int LIGHTS_COUNT = 11;

// LAYOUT BINDINGS AND LOCATIONS

// Uniform Buffer Object (UBO) per Toon shading
layout(binding = 0) uniform ToonUniformBufferObject
{
    mat4 mvpMat;  // Matrice Model-View-Projection
    mat4 mMat;    // Matrice Model
    mat4 nMat;    // Matrice Normale (trasposta e inversa della matrice model)
} ubo;

// Sampler per la texture
layout(binding = 1) uniform sampler2D texSampler;

// Global Uniform Buffer Object che contiene informazioni sulla luce
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

// Variabili di input per la posizione, normale e coordinate texture
layout(location = 0) in vec3 fragPos;
layout(location = 1) in vec3 fragNorm;
layout(location = 2) in vec2 fragTexCoord;

// Variabile di output del colore finale
layout(location = 0) out vec4 outColor;

// Funzione per determinare l'illuminazione Toon in base alla normale
float toon_shading(float diff) {
    if (diff > 0.9) {
        return 1.0;
    } else if (diff > 0.5) {
        return 0.75;
    } else if (diff > 0.2) {
        return 0.5;
    } else {
        return 0.25;
    }
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

// Funzione BRDF per Toon shading
vec3 BRDF(vec3 V, vec3 N, vec3 L, vec3 Md, vec3 Ms) {
    
    // decision parameters
    float cos_a = dot(N, L);
    vec3 R = 2.0f * N * cos_a - L;
    float cos_b = dot(V, R);
    
    // default percentages of diffuse and specular colors
    float pD = 0.0f;
    float pS = 0.0f;
    
    // decides percentage of diffuse color
    if(cos_a > 0.0f){
        if(cos_a <= 0.1f){
            pD = cos_a * 1.5f;
        }
        else if(cos_a <= 0.7f){
            pD = 0.15f;
        }
        else if(cos_a <= 0.8f){
            pD = 0.15f + (cos_a - 0.7f) * 8.5f;
        }
        else{
            pD = 1.0f;
        }
    }
    
    // decides percentage of specular color
    if(cos_b > 0.9f){
        if(cos_b <= 0.95f){
            pS = (cos_b - 0.9f) * 20.0f;
        }
        else{
            pS = 1.0f;
        }
    }
    
    return (pD * Md + pS * Ms);
}

void main() {
    vec3 Norm = normalize(fragNorm);
    vec3 EyeDir = normalize(gubo.eyePos - fragPos);
    vec3 Albedo = texture(texSampler, fragTexCoord).xyz;
    
    vec3 ambientLightDirection = gubo.ambientLightDir;
    vec4 ambientLightColor = gubo.ambientLightColor;

    vec3 LD;  // Light direction
    vec3 LC; // Light color

    vec3 RendEqSol = vec3(0);  // Result of lighting equation

    LD = point_light_dir(fragPos, 0);
    LC = point_light_color(fragPos, 0);
    RendEqSol += BRDF(EyeDir, Norm, LD, Albedo, vec3(1.0)) * LC * gubo.lightOn[0];
    
    LD = point_light_dir(fragPos, 1);
    LC = point_light_color(fragPos, 1);
    RendEqSol += BRDF(EyeDir, Norm, LD, Albedo, vec3(1.0)) * LC * gubo.lightOn[1];
    
    LD = point_light_dir(fragPos, 2);
    LC = point_light_color(fragPos, 2);
    RendEqSol += BRDF(EyeDir, Norm, LD, Albedo, vec3(1.0)) * LC * gubo.lightOn[2];
    
    LD = point_light_dir(fragPos, 3);
    LC = point_light_color(fragPos, 3);
    RendEqSol += BRDF(EyeDir, Norm, LD, Albedo, vec3(1.0)) * LC * gubo.lightOn[3];
    
    LD = point_light_dir(fragPos, 4);
    LC = point_light_color(fragPos, 4);
    RendEqSol += BRDF(EyeDir, Norm, LD, Albedo, vec3(1.0)) * LC * gubo.lightOn[4];
    
    LD = point_light_dir(fragPos, 5);
    LC = point_light_color(fragPos, 5);
    RendEqSol += BRDF(EyeDir, Norm, LD, Albedo, vec3(1.0)) * LC * gubo.lightOn[5];
    
    LD = point_light_dir(fragPos, 6);
    LC = point_light_color(fragPos, 6);
    RendEqSol += BRDF(EyeDir, Norm, LD, Albedo, vec3(1.0)) * LC * gubo.lightOn[6];
    
    LD = point_light_dir(fragPos, 7);
    LC = point_light_color(fragPos, 7);
    RendEqSol += BRDF(EyeDir, Norm, LD, Albedo, vec3(1.0)) * LC * gubo.lightOn[7];
    
    LD = spot_light_dir(fragPos, 8);
    LC = spot_light_color(fragPos, 8);
    RendEqSol += BRDF(EyeDir, Norm, LD, Albedo, vec3(1.0)) * LC * gubo.lightOn[8];
    
    LD = spot_light_dir(fragPos, 9);
    LC = spot_light_color(fragPos, 9);
    RendEqSol += BRDF(EyeDir, Norm, LD, Albedo, vec3(1.0)) * LC * gubo.lightOn[9];
    
    LD = spot_light_dir(fragPos, 10);
    LC = spot_light_color(fragPos, 10);
    RendEqSol += BRDF(Albedo, Norm, EyeDir, LD, vec3(1.0)) * LC * gubo.lightOn[10];

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

    // Final color calculation
    outColor = vec4(((ambientDiffuse + ambientSpecular * (1.0 - gubo.eyeDir.w)) * ambientLightColor.xyz) + ambientCorrection, 1.0) + vec4(RendEqSol, 1.0);
}
