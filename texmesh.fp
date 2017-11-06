#version 330

uniform sampler2D texture_unit;

in vec2 vVaryingTexCoords;

void main()
{
    gl_FragColor = texture2D(texture_unit, vVaryingTexCoords);
}
