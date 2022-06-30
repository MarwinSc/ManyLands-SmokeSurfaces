
#version 330 core
layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in VS_OUT {
    vec4 Color;
    vec3 Normal;
    vec4 Debug;
} gs_in[];

uniform mat4 projection;

out vec4 Debug;

const float MAGNITUDE = 0.05;

void GenerateLine(int index)
{
    Debug = vec4(gs_in[index].Normal, 1.0);
    //projection * 
    gl_Position = gl_in[index].gl_Position;
    EmitVertex();
    gl_Position = (gl_in[index].gl_Position + vec4(gs_in[index].Normal, 0.0) * MAGNITUDE);
    EmitVertex();
    EndPrimitive();
}

void main()
{

    GenerateLine(0); 
    GenerateLine(1); 
    GenerateLine(2);

}  