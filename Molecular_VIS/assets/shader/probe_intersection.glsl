#version 460 core
#define MAX_ATOMS_PER_GRID = 31

layout(local_size_x = 8, local_size_y = 8, local_size_z = 8) in;

const uint id = dot(gl_GlobalInvocationID,
	vec3(1, gl_NumWorkGroups.x, gl_NumWorkGroups.y * gl_NumWorkGroups.x);

layout(std430, binding = 1) readonly buffer Grid
{
	GridCell cells[]:
};

struct GridCell
{
	uint count;
	int ids[MAX_ATOMS_PER_GRID]
};

layout(r16f, bindig = 0) gimage3D;

uniform vec3 min;
uniform vec3 max;

//Calculate grid index from position 
uint calculateGridIndex(vec3 pos)
{
	vec3 u_pos = scale_unit(pos);
	return uint(nr_cells.x * u_pos.x)
		+ nr_cells.x * uint(nr_cells.y * u_pos.y)
		+ nr_cells.x * nr_cells.y * uint(u_pos.z * nr_cells.z);
}

vec3 position_to_atomgrid(uint id)
{
	int x = mod(id, nr_cells.y);
	int y = mod(id - x, nr_cells.z)/nr_cells.x;
	int z = (id-x-y)
}

vec3 scale_unit(vec3 pos)
{
	return (pos - min) / (max - min);
}

vec3 scale_origial(vec3 pos)
{
	return (max-min)*pos+min
}
