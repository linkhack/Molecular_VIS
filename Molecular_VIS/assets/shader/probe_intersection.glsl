#version 430 core
#define MAX_ATOMS_PER_GRID = 32

layout(local_size_x = 8, local_size_y = 8, local_size_z = 8) in;

const uint id = dot(gl_GlobalInvocationID,
	vec3(1, gl_NumWorkGroups.x, gl_NumWorkGroups.y * gl_NumWorkGroups.x);

layout(std430, binding = 1) buffer Grid
{
	GridCell cells[]:
};

struct GridCell
{
	uint count;
	int ids[MAX_ATOMS_PER_GRID]
};

uniform vec3 min;
uniform vec3 max;

int calculateGridIndex(vec3 pos)
{
	return cell_nr.x * normalize(pos.x)
		+ cell_nr.x * (cell_nr.y * normalize(pos.y))
		+ cell_nr.x * cell_nr.y * (normalize(pos.z) * cell_nr.z);
}
