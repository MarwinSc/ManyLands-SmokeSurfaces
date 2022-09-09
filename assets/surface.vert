#version 400 core
layout (location = 0) in vec4 aPos;
layout (location = 1) in vec4 aColor;
layout (location = 2) in vec4 aNormal;
layout (location = 3) in float opacity;

//for normals
out VS_OUT {
    vec3 Normal;
} vs_out;

out vec4 Color;
out vec3 Normal;
out vec3 FragPos;

uniform mat4 mvp;
uniform mat3 normalMatrix;

void main()
{   
    Color = vec4(aColor.xyz, opacity);
    Normal = normalMatrix * vec3(aNormal);
    FragPos = vec3(mvp * aPos);

    vs_out.Normal = normalMatrix * vec3(aNormal);

    gl_Position = mvp * aPos;
}

