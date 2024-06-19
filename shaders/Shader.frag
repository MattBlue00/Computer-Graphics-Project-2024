#version 450

layout(binding = 0) uniform sampler2D tex;

layout(location = 0) in vec2 fragUV;
layout(location = 1) in vec4 fragColor;

layout(location = 0) out vec4 outColor;

void main()
{
    outColor = vec4((texture(tex, fragUV).xyz * (1.0 - fragColor.w)) + (fragColor.xyz * fragColor.w), 1.0);
}

