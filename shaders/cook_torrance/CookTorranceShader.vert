// CookTorranceShader.vert

// DIFFUSION MODEL: LAMBERT
// REFLECTION MODEL: COOK-TORRANCE

#version 450

// Input layout
layout(location = 0) in vec3 inPosition;       // Vertice posizione
layout(location = 1) in vec3 inNormal;         // Normale del vertice
layout(location = 2) in vec2 inTexCoord;       // Coordinate texture

// Output layout
layout(location = 0) out vec3 fragPos;         // Posizione del frammento nello spazio del mondo
layout(location = 1) out vec3 fragNorm;        // Normale interpolata
layout(location = 2) out vec2 fragTexCoord;    // Coordinate texture interpolate

// Uniforms
layout(binding = 0, std140) uniform CookTorranceUniformBufferObject {
    mat4 mvpMat; // Model-View-Projection matrix
    mat4 mMat;   // Model matrix
    mat4 nMat;   // Normal matrix (transpose(inverse(modelMatrix)))
    float metalness;
    float roughness;
} ubo;

void main()
{
    // Trasformazioni
    fragPos = vec3(ubo.mMat * vec4(inPosition, 1.0));   // Posizione nello spazio del mondo
    fragNorm = mat3(ubo.nMat) * inNormal;              // Trasformazione delle normali
    fragTexCoord = inTexCoord;                         // Passaggio delle coordinate texture

    gl_Position = ubo.mvpMat * vec4(inPosition, 1.0);  // Calcolo della posizione finale nel clip space
}
