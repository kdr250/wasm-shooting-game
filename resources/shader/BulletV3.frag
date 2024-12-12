#version 330

uniform vec2 uWindowSize;
uniform int uBulletCount;
uniform vec2 uBulletPositions[320];
uniform vec2 uBulletSizes[320];
uniform vec3 uBulletColors[320];

in vec2 inPos;

const float radius = 8.0;

out vec4 outColor;

void main()
{
    vec2 halfWindow = uWindowSize * 0.5;

    vec2 fragCoord = gl_FragCoord.xy;
    fragCoord.y = uWindowSize.y - fragCoord.y;

    vec4 resultColor = vec4(0.0);
    for (int i = 0; i < uBulletCount; ++i)
    {
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

    outColor = resultColor;
}
