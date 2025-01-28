precision mediump float;
precision mediump int;

uniform vec2 uWindowSize;
uniform int uBulletCount;
uniform vec2 uBulletPositions[320];
uniform vec2 uBulletSizes[320];
uniform vec3 uBulletColors[320];

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;

out vec2 inPos;

void main()
{
    gl_Position = vec4(inPosition, 1.0);
    inPos = inPosition.xy;
}
