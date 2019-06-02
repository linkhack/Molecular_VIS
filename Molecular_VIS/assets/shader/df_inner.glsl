#version 460 core
layout(local_size_x = 3, local_size_y = 3, local_size_z = 3) in;

layout(r32f, binding = 0) uniform restrict image3D SESTexture;
layout(r16f, binding = 1) uniform restrict readonly image3D borderClassification;

shared float values[5][5][5];
shared float classification[5][5][5];

const uvec3 texPos = gl_GlobalInvocationID;
const uvec3 blockCoords = gl_LocalInvocationID;

uniform float texRadius;
void main()
{
	// read into shared memory
	ivec3 coords = ivec3(2 * blockCoords);
	ivec3 texSize = imageSize(SESTexture);
	if (any(greaterThanEqual(texPos, texSize))) return;
	for (int i = 0; i <= 1; ++i)
	{
		for (int j = 0; j <= 1; ++j)
		{
			for (int k = 0; k <= 1; ++k)
			{
				ivec3 sharedCoords = coords + ivec3(i, j, k);
				ivec3 globalCoords = ivec3(texPos) + (coords-ivec3(blockCoords)) + ivec3(i,j,k);
				if (all(greaterThanEqual(globalCoords, ivec3(0))) && all(lessThan(globalCoords, texSize)))
				{
					values[sharedCoords.x][sharedCoords.y][sharedCoords.z]= imageLoad(SESTexture, ivec3(globalCoords)).r;
					classification[sharedCoords.x][sharedCoords.y][sharedCoords.z]= imageLoad(borderClassification, ivec3(globalCoords)).r;
				}
				else
				{
					values[sharedCoords.x][sharedCoords.y][sharedCoords.z] = 0.0f;
					classification[sharedCoords.x][sharedCoords.y][sharedCoords.z] = 1.0f;
				}
			}
		}
	}
	memoryBarrier();
	barrier();
	float maxValue = -10000.0f;
	ivec3 sharedCenter = ivec3(blockCoords) + ivec3(1);
	if (classification[sharedCenter.x][sharedCenter.y][sharedCenter.z] <0)
	{
		for (int i = -1; i <= 1; ++i)
		{
			for (int j = -1; j <= 1; ++j)
			{
				for (int k = -1; k <= 1; ++k)
				{
					ivec3 sharedCoords = sharedCenter + ivec3(i, j, k);
					maxValue = max(maxValue, values[sharedCoords.x][sharedCoords.y][sharedCoords.z] - texRadius * sqrt(i * i + j * j + k * k));
				}
			}
		}
		imageStore(SESTexture, ivec3(texPos)+ivec3(1), vec4(maxValue));
	}
	
}