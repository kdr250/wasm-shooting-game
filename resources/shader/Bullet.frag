#ifdef GL_ES
precision mediump float;
#endif

uniform vec2 uWindowSize;
uniform int uBulletCount;
uniform vec2 uBulletPositions[320];
uniform vec2 uBulletSizes[320];
uniform vec3 uBulletColors[320];

varying vec2 inPos;

const float radius = 8.0;

void main()
{
    vec2 halfWindow = uWindowSize * 0.5;

    vec2 fragCoord = gl_FragCoord.xy;
    fragCoord.y = uWindowSize.y - fragCoord.y;

    vec4 resultColor = vec4(0.0);
    for (int i = 0; i < 320; ++i)
    {
        if (i >= uBulletCount)
        {
            break;
        }

        vec2 uBulletPosition = uBulletPositions[i];
        vec2 uBulletSize = uBulletSizes[i];
        vec3 uBulletColor = uBulletColors[i];

        float positionX = (uBulletPosition.x - halfWindow.x) / halfWindow.x;
        float positionY = (uBulletPosition.y - halfWindow.y) / halfWindow.y * -1.0;
        vec2 diff = (uBulletSize / uWindowSize) * 0.5;

        vec2 finalPosition = vec2(positionX, positionY) + inPos.xy * diff;

        vec2 centerPosition = uBulletPosition.xy;
        vec3 bulletColor = uBulletColor;

        float dist = distance(centerPosition, fragCoord);

        float strength = radius / dist;
        vec3 color = bulletColor * strength;
        resultColor += vec4(color, strength);
    }

    gl_FragColor = resultColor;
}
