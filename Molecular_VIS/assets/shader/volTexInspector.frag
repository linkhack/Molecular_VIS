#version 460 core
out vec4 FragColor;

in vec2 TexCoords;

uniform float slice;
uniform float probeRadius;
uniform float texRadius;

uniform sampler3D SESTexture;

void main()
{
   float value = (texture(SESTexture, vec3(TexCoords,slice)).r+12.f)/11.f;
   FragColor = vec4(vec3(value),1.0f);
}