#version 330 core
layout (location = 0) in vec4 aPos;
layout (location = 1) in vec4 aColor;
layout (location = 2) in vec3 aNormal;

out VS_OUT {
    vec4 Color;
    vec3 Normal;
    vec4 Debug;
} vs_out;

uniform mat4 mvp;
uniform mat3 normalMatrix;

void main()
{
    vs_out.Color = aColor;
    vs_out.Normal = normalMatrix * aNormal;
    
    vs_out.Debug = vec4(aPos);

    gl_Position = mvp * aPos;
}
