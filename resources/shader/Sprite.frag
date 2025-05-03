#version 330

precision highp float;

in vec2 fragTexCoord;

uniform sampler2D uTexture;

out vec4 outColor;

void main()
{
    outColor = texture(uTexture, fragTexCoord);
}
