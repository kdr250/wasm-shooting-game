#version 330

in vec2 windowSize;
in vec2 centerPos;
in vec3 bulletColor;

const float radius = 8.0;

out vec4 outColor;

void main()
{
    vec2 fragCoord = gl_FragCoord.xy;
    fragCoord.y = windowSize.y - fragCoord.y;
    float dist = distance(centerPos, fragCoord);

    float strength = radius / dist;
    vec3 color = bulletColor * strength;
    outColor = vec4(color, strength);
}
