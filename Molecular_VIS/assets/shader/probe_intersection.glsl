#version 460 core
#define MAX_ATOMS_PER_GRID 31
#extension GL_ARB_shader_group_vote : enable

layout(local_size_x = 8, local_size_y = 8, local_size_z = 8) in;

const uint id = uint(dot(gl_GlobalInvocationID,
	vec3(1, gl_NumWorkGroups.x, gl_NumWorkGroups.y * gl_NumWorkGroups.x)));

const uvec3 texPos = gl_GlobalInvocationID;

struct GridCell
{
	uint count;
	uint ids[MAX_ATOMS_PER_GRID];
};

struct AtomStruct
{
	vec3 position;
	float radius;
};

layout(std430, binding = 0) readonly buffer Atoms
{
	AtomStruct atoms[];
};

layout(std430, binding = 1) readonly buffer Grid
{
	GridCell cells[];
};

layout(r32f, binding = 0) uniform coherent image3D SESTexture;
layout(r32f, binding = 1) uniform coherent image3D borderClassification;

uniform vec3 grid_min;
uniform vec3 grid_max;
uniform uvec3 nr_cells;
uniform float probeRadius;
uniform float texRadius;

// Function prototypes
uint calculateGridIndex(vec3 pos);
vec3 atomGridPosition(uint id);
vec3 textureToWorld(ivec3 tex_pos);
vec3 scale_unit(vec3 pos);
vec3 scale_original(vec3 pos);


//Calculate grid index from position 
uint calculateGridIndex(vec3 pos)
{
	vec3 u_pos = scale_unit(pos);
	return uint(nr_cells.x * u_pos.x)
		+ nr_cells.x * uint(nr_cells.y * u_pos.y)
		+ nr_cells.x * nr_cells.y * uint(u_pos.z * nr_cells.z);
}
// Calculates position from gird index
vec3 atomGridPosition(uint id)
{
	// x,y,z of grid
	float x = float(mod(id, nr_cells.x));
	float y = float(mod(id/nr_cells.x, nr_cells.y));
	float z = float(id / (nr_cells.x*nr_cells.y));
	// x,y,z in unit (should we add 0.5?)
	x = x / float(nr_cells.x);
	y = y / float(nr_cells.y);
	z = z / float(nr_cells.z);
	// scale to min/max
	return scale_original(vec3(x, y, z));
}

vec3 textureToWorld(uvec3 tex_pos)
{
	return scale_original(vec3(tex_pos) / vec3(imageSize(SESTexture)));
}

vec3 scale_unit(vec3 pos)
{
	return (pos - grid_min) / (grid_max - grid_min);
}

vec3 scale_original(vec3 pos)
{
	return (grid_max - grid_min)*pos + grid_min;
}

void main()
{
	bool isBorder = false;
	bool isInside = false;
	bool isOutside = false;
	int insideOutClassification = 1; //-1 inside, 0 border, +1 outside
	ivec3 texSize = imageSize(SESTexture);
	if (!all(lessThan(texPos, texSize))) return;
	vec3 worldPos = textureToWorld(texPos);
	uint atomGridId = calculateGridIndex(worldPos);
	int nr_atoms = atoms.length();
	int count = 0;
	// Check for intersections
	for (int i = -1; i <= 1; ++i) // x
	{
		for (int j = -1; j <= 1; ++j) // y
		{
			for (int k = -1; k <= 1; ++k) // z
			{
				int checkIndex = int(atomGridId) + i + int(nr_cells.x)*j + int(nr_cells.x)*int(nr_cells.y)*k;
				if (checkIndex >= 0 && checkIndex < nr_atoms) // valid index (still in grid)
				{
					GridCell currentCell = cells[checkIndex];
					if (currentCell.count > 0) // has atom in grid
					{
						for (int atomIndex = 0; atomIndex < MAX_ATOMS_PER_GRID; ++atomIndex)
						{
							if (atomIndex >= currentCell.count) break;
							uint ssboIndex = currentCell.ids[atomIndex];
							AtomStruct currentAtom = atoms[ssboIndex];
							float dist2 = dot(worldPos - currentAtom.position, worldPos - currentAtom.position);
							if (dist2 < (currentAtom.radius-texRadius)*(currentAtom.radius-texRadius))
							{
								//Inside
								insideOutClassification = min(insideOutClassification, -1);
								//break;
							}
							else if (dist2 > (currentAtom.radius + probeRadius)*(currentAtom.radius + probeRadius))
							{
								//Outside
								insideOutClassification = min(insideOutClassification, 1);
							}
							else
							{
								//Border
								insideOutClassification = min(insideOutClassification, 0);
							}
						}
					}
				}
			}
		}
	}
	isInside = (insideOutClassification == -1);
	isBorder = (insideOutClassification == 0);
	isOutside = (insideOutClassification == 1);
	if (isOutside)
	{
		imageStore(SESTexture, ivec3(texPos), vec4(probeRadius,0,0,1));
	}
	if (isInside)
	{
		imageStore(SESTexture, ivec3(texPos), vec4(-texRadius,0,0,1));
	}
	if (isBorder)
	{
		imageStore(SESTexture, ivec3(texPos), vec4(-texRadius, 0, 0, 1));
	}
	imageStore(borderClassification, ivec3(texPos), vec4(insideOutClassification,0,0,1));
}