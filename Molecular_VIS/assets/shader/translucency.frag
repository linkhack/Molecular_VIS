#version 460 core
#define PI 3.1415926
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D SESTexture;
uniform sampler2D fc;
uniform sampler2D offset;
uniform bool refractionOn;
uniform bool translucencyOn;
uniform int kernelSize = 2;
vec2 offsetCoords; 
const vec2 texOffset = 1.0/textureSize(SESTexture,0);

float weight(int i,int j)
{
	float sig = texture(fc,vec2(offsetCoords.x+texOffset.x*i,offsetCoords.y+texOffset.y*j)).r;
	return 1/(2*PI*sig*sig)*exp(-(i*i+j*j)/2*sig*sig);
}

void main()
{
	float totalWeight=0;
	vec3 result;
	if(refractionOn)
	{
		offsetCoords = TexCoords+texture(offset,TexCoords).xy;
	} else
	{
		offsetCoords = TexCoords;
	}
	if(translucencyOn)
	{
		result=vec3(0.0f);
		for(int i=-kernelSize;i<kernelSize;i++)
		{
			for(int j=-kernelSize;j<kernelSize;j++)
			{
				float w = weight(i,j);
				//float w = 0.5;
				result += texture(SESTexture,offsetCoords+vec2(texOffset.x*i,texOffset.y*j)).rgb*w;
				//result+=vec3(0.01f);
				totalWeight += w;
			}
		}
		FragColor = vec4(result/totalWeight,1.0f);
		return;
	} else
	{	
		FragColor = vec4(texture(SESTexture,offsetCoords));
		return;
	}
}