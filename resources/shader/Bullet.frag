#ifdef GL_ES
precision mediump float;
#endif

varying vec2 windowSize;
varying vec2 centerPos;
varying vec3 bulletColor;

const float radius = 8.0;

void main()
{
    vec2 fragCoord = gl_FragCoord.xy;
    fragCoord.y = windowSize.y - fragCoord.y;
    float dist = distance(centerPos, fragCoord);

    float strength = radius / dist;
    vec3 color = bulletColor * strength;
    gl_FragColor = vec4(color, strength);
}
