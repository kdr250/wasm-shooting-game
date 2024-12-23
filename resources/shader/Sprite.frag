#ifdef GL_ES
precision highp float;
#endif

varying vec2 fragTexCoord;

uniform sampler2D uTexture;

void main()
{
    gl_FragColor = texture2D(uTexture, fragTexCoord);
}
