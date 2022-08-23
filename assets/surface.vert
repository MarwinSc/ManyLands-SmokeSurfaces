#version 400 core
layout (location = 0) in vec4 aPos;
layout (location = 1) in vec4 aColor;
layout (location = 2) in vec4 aNormal;
layout (location = 3) in float curvature;


out VS_OUT {
    vec4 Color;
    vec3 Normal;
    vec4 Debug;
} vs_out;

uniform mat4 mvp;
uniform mat3 normalMatrix;

void main()
{   
    //if (curvature < 0.5){
    //    vs_out.Color = vec4(1.0-curvature,0.0,0.0,curvature);
    //}else{
    //    vs_out.Color = vec4(0.0,curvature,0.0,curvature);
    //}
    
    //vs_out.Color = vec4(curvature,curvature,curvature,curvature);

    vs_out.Color = vec4(aColor.xyz, curvature);
      
    vs_out.Normal = normalMatrix * vec3(aNormal);
    
    vs_out.Debug = vec4(aPos);

    gl_Position = mvp * aPos;
}

