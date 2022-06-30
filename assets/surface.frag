#version 330 core
out vec4 FragColor;
in vec4 Color;
in vec3 Normal;
in vec3 FragPos;
in vec3 CameraPos;

void main()
{
   
//    FragColor = vec4(Color.x, Color.y, Color.z, Color.w);
    //FragColor = vec4(Normal,1.0);

    vec3 LightPos = CameraPos; 
    vec3 lightColor = vec3(1.0,1.0,1.0);

    float ambientStrength = 0.5;
    vec3 ambient = ambientStrength * lightColor;
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(LightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
            
    vec3 result = (ambient + diffuse) * vec3(Color.x,Color.y,Color.z);
    FragColor = vec4(result, Color.w); //
    //FragColor = vec4(1.0,0.0,0.0,1.0);
}
