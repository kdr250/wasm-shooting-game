#ifdef GL_ES
precision mediump float;
#endif

uniform vec2 uWindowSize;
uniform int uBulletCount;
uniform vec2 uBulletPositions[320];
uniform vec2 uBulletSizes[320];
uniform vec3 uBulletColors[320];

varying vec2 inPos;

const float radius = 16.0;

void main()
{
    vec2 fragCoord = gl_FragCoord.xy;
    fragCoord.y = uWindowSize.y - fragCoord.y;

    float nearestDist = 10000.0;
    vec3 nearestColor = uBulletColors[0];

    for (int i = 0; i < 320; ++i)
    {
        if (i >= uBulletCount)
        {
            break;
        }

        vec2 uBulletPosition = uBulletPositions[i];
        vec2 centerPosition = uBulletPosition.xy;

        float dist = distance(centerPosition, fragCoord);

        if (dist < nearestDist)
        {
            nearestDist = dist;
            nearestColor = uBulletColors[i];
        }
    }

    float strength = radius / nearestDist;
    vec3 color = nearestColor * strength;

    gl_FragColor = vec4(color, strength);
}
