
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

vec3 calculate_normal(vec3 v1, vec3 v2, vec3 v3){
    vec3 e1 = v1 - v2;
    vec3 e2 = v3 - v2;
    return cross(e1,e2);
}

void GenerateLine()
{
    vec4 position = gl_in[0].gl_Position;
    vec3 v1 = vec3(position.x, position.y, position.z);
    position = gl_in[1].gl_Position;
    vec3 v2 = vec3(position.x, position.y, position.z);
    position = gl_in[2].gl_Position;
    vec3 v3 = vec3(position.x, position.y, position.z);
    vec3 normal = normalize(calculate_normal(v1, v2, v3));
    //vec3 normal = gs_in[0].Normal;

    Debug = vec4(gs_in[0].Normal, 1.0);

    vec3 Position = (v1 + v2 + v3) / 3.0;
    //projection * 
    gl_Position = vec4(Position, 1.0);
    EmitVertex();
    gl_Position = (vec4(Position, 1.0) + vec4(normal, 0.0) * MAGNITUDE);
    EmitVertex();
    EndPrimitive();
}

void main()
{

    GenerateLine(); 

}  