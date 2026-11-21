#version 460 core

// GLSL fragment shader
in vec3 eyeSpacePos;
in vec3 worldSpaceNormal;
in vec3 eyeSpaceNormal;

uniform vec4 deepColor;
uniform vec4 shallowColor;
uniform vec4 skyColor;
uniform vec3 lightDir;

out vec4 FragColor;

void main()
{
    vec3 eyeVector              = normalize(eyeSpacePos);
    vec3 eyeSpaceNormalVector   = normalize(eyeSpaceNormal);
    vec3 worldSpaceNormalVector = normalize(worldSpaceNormal);

    float facing    = max(0.0, dot(eyeSpaceNormalVector, -eyeVector));
    float fresnel   = pow(1.0 - facing, 5.0); // Fresnel approximation
    float diffuse   = max(0.0, dot(worldSpaceNormalVector, lightDir));
    
   vec4 waterColor = mix(shallowColor, deepColor, facing);
    // vec4 waterColor = deepColor;
    
//    gl_FragColor = gl_Color;
//    gl_FragColor = vec4(fresnel);
//    gl_FragColor = vec4(diffuse);
//    gl_FragColor = waterColor;
//    FragColor = waterColor*diffuse;
    FragColor = waterColor*diffuse + skyColor*fresnel;
    // FragColor = vec4(1, 0, 0, 1);
}