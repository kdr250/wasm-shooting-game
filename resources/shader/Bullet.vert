#ifdef GL_ES
precision mediump float;
precision mediump int;
#endif

uniform vec2 uWindowSize;
uniform int uBulletCount;
uniform vec2 uBulletPositions[320];
uniform vec2 uBulletSizes[320];
uniform vec3 uBulletColors[320];

attribute vec3 inPosition;
attribute vec2 inTexCoord;

varying vec2 inPos;

void main()
{
    gl_Position = vec4(inPosition, 1.0);
    inPos = inPosition.xy;
}
