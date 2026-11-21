#version 460 core

out vec4 FragColor;

float random(in vec2 _st)
{
    return fract(sin(dot(_st.xy, vec2(12.9898, 78.233))) * 43758.5453123);
}

float noise(in vec2 _st)
{
    vec2 i = floor(_st);
    vec2 f = fract(_st);
    float a = random(i);
    float b = random(i + vec2(1.0, 0.0));
    float c = random(i + vec2(0.0, 1.0));
    float d = random(i + vec2(1.0, 1.0));
    vec2 u = f * f * (3.0 - 2.0 * f);
    return mix(a, b, u.x) + (c-a) * u.y * (1.0 - u.x) + (d-b) * u.x * u.y;
}

void main()
{
    vec2 st = gl_FragCoord.xy / 1000.0;
    vec3 color = vec3(st, 0.5);
    // color = vec3(noise(st));
    color = vec3(random(st));
    FragColor = vec4(color, 1.0);
    // FragColor = vec4(1.0, 1.0, 0.0, 0.0);
}
