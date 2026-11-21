#version 460 core

// GLSL vertex shader
layout(location=0) in vec3 a_vertex;
layout(location=1) in float a_height;
layout(location=2) in vec2 a_slope;

out vec3 eyeSpacePos;
out vec3 worldSpaceNormal;
out vec3 eyeSpaceNormal;

uniform float heightScale; // = 0.5;
uniform float chopiness;   // = 1.0;
uniform vec2  size;        // = vec2(256.0, 256.0);

uniform mat4 modelMatrixUniform;
uniform mat4 viewMatrixUniform;
uniform mat4 projectionMatrixUniform;

void main()
{
    // vec2 size = vec2(256.0, 256.0);

    float height     = a_height.x;
    vec2  slope      = a_slope.xy;

    // calculate surface normal from slope for shading
	vec3 normal      = normalize(cross( vec3(0.0, slope.y*heightScale, 2.0 / size.x), vec3(2.0 / size.y, slope.x*heightScale, 0.0)));
    worldSpaceNormal = normal;

    // calculate position and transform to homogeneous clip space
    vec4 pos         = vec4(a_vertex.x, height * heightScale, a_vertex.z, 1.0);
    mat4 modelViewmatrix = modelMatrixUniform * viewMatrixUniform;
    gl_Position      = projectionMatrixUniform * viewMatrixUniform * modelMatrixUniform * pos;
    
    eyeSpacePos      = (modelViewmatrix * pos).xyz;

    mat3 normalMatrix = mat3(transpose(inverse(modelMatrixUniform * viewMatrixUniform)));
    eyeSpaceNormal   = (normalMatrix * normal).xyz;
}
