#version 450

layout(location = 0) in vec2 inPosition;

layout (location = 0) out vec3 outPosition;

vec2 vertices[3] = 
{
    vec2(-0.5, 0.5),
    vec2(0, -0.5),
    vec2(0.5, 0.5)
};

void main()
{
    gl_Position = vec4(vertices[gl_VertexIndex], 1.0, 1.0);

	outPosition = vec3(vertices[gl_VertexIndex], 0.0);
    //gl_Position = vec4(inPosition, 1.0, 1.0);
}