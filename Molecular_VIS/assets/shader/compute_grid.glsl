#version 460 core
#define MAX_ATOMS_PER_GRID 31

layout(local_size_x = 1024, local_size_y = 1, local_size_z = 1) in;

const uint id = uint(dot(gl_GlobalInvocationID,
	vec3(1, gl_NumWorkGroups.x, gl_NumWorkGroups.y * gl_NumWorkGroups.x)));

struct AtomStruct 
{
	vec3 position;
	float radius;
};

struct GridCell
{
	uint count;
	uint ids[MAX_ATOMS_PER_GRID];
};

layout(std430, binding = 0) readonly buffer Atoms
{
	AtomStruct atoms[];
};

layout(std430, binding = 1) buffer Grid
{
	GridCell cells[];
};

uniform int nrAtoms;

// Size of grid, i.e how many cells per dimension
uniform uvec3 nr_cells;
uniform vec3 min;
uniform vec3 max;

//function prototypes

uint calculateGirdIndex(vec3 pos);
vec3 scale_unit(vec3 pos);
void constructGrid();

//Calculate grid index from position 
uint calculateGridIndex(vec3 pos)
{
	vec3 u_pos = scale_unit(pos);
	return uint(nr_cells.x * u_pos.x)
		+ nr_cells.x * uint(nr_cells.y * u_pos.y)
		+ nr_cells.x * nr_cells.y * uint(u_pos.z * nr_cells.z);
}

vec3 scale_unit(vec3 pos) 
{
	return (pos - min) / (max - min);
}

void constructGrid()
{
	if (id >= nrAtoms) return;
	AtomStruct atom = atoms[id];
	uint gridId = calculateGridIndex(atom.position);
	uint id_pos = atomicAdd(cells[gridId].count,1u);
	if (id_pos >= MAX_ATOMS_PER_GRID) return;
	cells[gridId].ids[id_pos] = id;
}

void main()
{
	constructGrid();
}
