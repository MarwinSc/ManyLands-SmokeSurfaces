#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VS_OUT {
    vec4 Color;
    vec3 Normal;
    vec4 Debug;
} gs_in[];

out vec4 Color;
out vec3 Normal;
out vec3 FragPos;
out vec3 CameraPos;
out vec4 Debug;

uniform vec3 camera;
uniform float surface_height;

float triangle_area(vec3 v1, vec3 v2, vec3 v3)
{
    vec3 c1 = (v2 - v1);
    vec3 c2 = (v3 - v1);
    vec3 r = cross(c1,c2);
    return sqrt(r.x * r.x + r.y * r.y + r.z * r.z) / 2.0;
}

float angle_between_vectors(vec3 v1, vec3 v2)
{
    v1 = normalize(v1);
    v2 = normalize(v2);
    float d = dot(v1,v2);
    return acos(d) * (180.0 / 3.141592653589793238463);
}

float calculate_opacity(vec3 v1, vec3 v2, vec3 v3, vec3 camera, vec3 normal, float surface_height, float curvature)
{
    //alpha density
    float area = triangle_area(v1,v2,v3);
    float angle = angle_between_vectors(camera,normal);
    float density = clamp(surface_height / (area * angle), 0.0f, 1.0f);

    //alpha shape
    float d0 = length(v3-v2);
    float d1 = length(v1-v3);
    float d2 = length(v2-v1);
    float max_distance = max(d0,max(d1,d2));
    float exponent = 0.5;
    float shape = pow((4.0 * area) / (sqrt(3) * max_distance), exponent);

    return clamp(density * shape * curvature , 0.0, 1.0);
}

void main(){
    
    vec4 position = gl_in[0].gl_Position;
    vec3 v1 = vec3(position.x, position.y, position.z);
    position = gl_in[1].gl_Position;
    vec3 v2 = vec3(position.x, position.y, position.z);
    position = gl_in[2].gl_Position;
    vec3 v3 = vec3(position.x, position.y, position.z);

    vec3 normal = gs_in[0].Normal;
    //vec3 normal = gs_in[0].Normal;
    float opacity = calculate_opacity(v1, v2, v3, camera, normal, surface_height, gs_in[0].Color.w);
    gl_Position = gl_in[0].gl_Position;
    vec4 color = gs_in[0].Color;
    Color = vec4(color.x, color.y, color.z, opacity);
    //Color = vec4(normal, 1.0);
    FragPos = v1;
    Normal = normal;
    CameraPos = camera;
    Debug = gs_in[0].Debug;
    EmitVertex();

    normal = gs_in[1].Normal;
    opacity = calculate_opacity(v1, v2, v3, camera, normal, surface_height, gs_in[1].Color.w);
    gl_Position = gl_in[1].gl_Position;
    color = gs_in[1].Color;
    Color = vec4(color.x, color.y, color.z, opacity);
    //Color = vec4(normal, 1.0);
    FragPos = v2;
    Normal = normal;
    CameraPos = camera;
    Debug = gs_in[1].Debug;
    EmitVertex();

    normal = gs_in[2].Normal;
    opacity = calculate_opacity(v1, v2, v3, camera, normal, surface_height, gs_in[2].Color.w);
    gl_Position = gl_in[2].gl_Position;
    color = gs_in[2].Color;
    Color = vec4(color.x, color.y, color.z, opacity);
    //Color = vec4(normal, 1.0);
    FragPos = v3;
    Normal = normal;
    CameraPos = camera;
    Debug = gs_in[2].Debug;
    EmitVertex();

    EndPrimitive();
}