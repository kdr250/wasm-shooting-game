#ifdef GL_ES
precision mediump float;
#endif

uniform vec2 uWindowSize;
uniform vec2 uTextureSize;
uniform vec2 uTexturePosition;
uniform float uTextureScale;

attribute vec3 inPosition;
attribute vec2 inTexCoord;

varying vec2 fragTexCoord;

void main()
{
    vec2 halfWindow = uWindowSize * 0.5;
    float positionX = (uTexturePosition.x - halfWindow.x) / halfWindow.x;
    float positionY = (uTexturePosition.y - halfWindow.y) / halfWindow.y * -1.0;
    vec2 diff = (uTextureSize / uWindowSize) * 0.5 * uTextureScale;

    vec2 finalPosition = vec2(positionX, positionY) + inPosition.xy * diff;

    gl_Position = vec4(finalPosition, inPosition.z, 1.0);
    fragTexCoord = inTexCoord;
}
