// TextShader.frag

#version 450

layout(binding = 0) uniform sampler2D texSampler;

layout(location = 0) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

void main()
{
    vec4 Tx = texture(texSampler, fragTexCoord);
    outColor = ((vec4(1.0) * Tx.x) + (vec4(0.0, 0.0, 0.0, 1.0) * Tx.y)) + (vec4(0.0, 0.0, 0.0, 0.5) * Tx.z);
}

