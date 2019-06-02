#version 460 core

layout(local_size_x = 8, local_size_y = 8, local_size_z = 8) in;

const uvec3 texPos = gl_GlobalInvocationID;

uniform float probeRadius;
uniform float texRadius;

layout(r32f, binding = 0) uniform restrict image3D SESTexture;
layout(r16f, binding = 1) uniform restrict readonly image3D borderClassification;

void main()
{
	ivec3 texSize = imageSize(SESTexture);
	if (!all(lessThan(texPos, texSize))) return;
	float classification = imageLoad(borderClassification, ivec3(texPos)).r;
	if (classification == 0)
	{
		float nbhdRadius = probeRadius + texRadius;
		int voxelRadius = int(nbhdRadius / texRadius) + 1;
		int voxelRadius2 = voxelRadius * voxelRadius;
		bool hasNeighbor = false;
		float minDistance2=10000.0f;

		// TODO: check if sqrt makes it smaller (maybe cube is better)
		for (int i = -voxelRadius; i <= voxelRadius; ++i)
		{
			int secondVoxelRadius = int(sqrt(voxelRadius2 - i * i)) + 1;
			for (int j = -secondVoxelRadius; j <= secondVoxelRadius; ++j)
			{
				int thirdVoxelRadius = int(sqrt(voxelRadius2 - i * i - j * j)) + 1;
				for (int k = -thirdVoxelRadius; k <= thirdVoxelRadius; ++k)
				{
					ivec3 comparePosition = ivec3(texPos) + ivec3(i, j, k);
					if (all(greaterThanEqual(comparePosition, ivec3(0))) && all(lessThan(comparePosition, texSize)))
					{
						float otherClassification = imageLoad(borderClassification, comparePosition).r;
						if (otherClassification>0) 
						{
							hasNeighbor = true;
							float distance2 = i * i + j * j + k * k;
							if (distance2 < minDistance2)
							{
								minDistance2 = distance2;
								
							}
						}

					}
				}
			}
		}
		if (hasNeighbor)
		{
			imageStore(SESTexture, ivec3(texPos), vec4(probeRadius - texRadius*sqrt(minDistance2)));
		}

	}
}