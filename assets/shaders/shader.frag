#version 450

layout(location = 0) in vec3 inPosition;

layout(location = 0) out vec4 fragmentColor;

void main()
{
    fragmentColor = vec4(inPosition.xyz, 1.0);
}