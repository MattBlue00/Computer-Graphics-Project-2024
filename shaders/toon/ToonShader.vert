// ToonShader.vert

#version 450

// Uniform Buffer Object (UBO) per Toon shading
layout(binding = 0, std140) uniform ToonUniformBufferObject
{
    mat4 mvpMat;  // Matrize Model-View-Projection
    mat4 mMat;    // Matrize Model
    mat4 nMat;    // Matrize Normal (trasposta e inversa della matrice model)
} ubo;

// Attributi degli input (dati dal vertex)
layout(location = 0) in vec3 inPosition;  // Posizione del vertice
layout(location = 1) in vec3 inNormal;    // Normale del vertice
layout(location = 2) in vec2 inTexCoord;  // Coordinate texture del vertice

// Variabili di output per il frammento
layout(location = 0) out vec3 fragPos;    // Posizione del frammento
layout(location = 1) out vec3 fragNorm;   // Normale del frammento
layout(location = 2) out vec2 fragTexCoord; // Coordinate texture per il frammento

void main() {
    // Calcola la posizione del vertice in spazio mondo usando la matrice model
    fragPos = vec3(ubo.mMat * vec4(inPosition, 1.0));

    // Calcola la normale trasformata in spazio mondo usando la matrice normal
    fragNorm = normalize(mat3(ubo.nMat) * inNormal);

    // Calcola la posizione finale usando la matrice MVP (Model-View-Projection)
    gl_Position = ubo.mvpMat * vec4(fragPos, 1.0);
    
    // Passa anche le coordinate texture
    fragTexCoord = inTexCoord;
}
