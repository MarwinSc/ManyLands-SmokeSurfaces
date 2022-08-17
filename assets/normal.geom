
#version 330 core
layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in VS_OUT {
    vec4 Color;
    vec3 Normal;
    vec4 Debug;
} gs_in[];

out vec4 Debug;

const float MAGNITUDE = 0.04;


void GenerateLine(int index)
{
    vec4 position = gl_in[index].gl_Position;

    vec3 normal = gs_in[index].Normal;

    Debug = position;

    //projection * 
    gl_Position = position;
    EmitVertex();
    gl_Position = (position + vec4(normal, 0.0) * MAGNITUDE);
    EmitVertex();
    EndPrimitive();
}

void main()
{
    GenerateLine(0); 
    GenerateLine(1); 
    GenerateLine(2); 
}  