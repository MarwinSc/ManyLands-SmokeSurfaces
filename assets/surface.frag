#version 400 core
// shader outputs
layout (location = 0) out vec4 accum;
layout (location = 1) out float reveal;

//out vec4 FragColor;

in vec4 Color;
in vec3 Normal;
in vec3 FragPos;
in vec3 CameraPos;

void main()
{
   
//    FragColor = vec4(Color.x, Color.y, Color.z, Color.w);
    //FragColor = vec4(Normal,1.0);

    //ambient
    vec3 LightPos = CameraPos; 
    vec3 lightColor = vec3(1.0,1.0,1.0);

    float ambientStrength = 0.4;
    vec3 ambient = ambientStrength * lightColor;
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(LightPos - FragPos);
    float diff = max(abs(dot(norm, lightDir)), 0.0);
    vec3 diffuse = diff * lightColor;
            
    vec3 resultColor = vec3(Color.x,Color.y,Color.z)* (ambient + diffuse);
    //FragColor = vec4(result, Color.w); //
    
    // weight function
    float weight = clamp(pow(min(1.0, Color.w * 10.0) + 0.01, 3.0) * 1e8 * pow(1.0 - gl_FragCoord.z * 0.9, 3.0), 1e-2, 3e3);

    // store pixel color accumulation
    accum = vec4(Color.xyz * Color.w, Color.w) * weight;
    //accum = vec4(resultColor.xyz * Color.w, Color.w) * weight;

    // store pixel revealage threshold
    reveal = Color.w;

}
