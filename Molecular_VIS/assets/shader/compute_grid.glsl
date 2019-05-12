#version 430 core
#define MAX_ATOMS_PER_GRID 32

layout(local_size_x = 8, local_size_y = 8, local_size_z = 8) in;

const uint id = uint(dot(gl_GlobalInvocationID,
	vec3(1, gl_NumWorkGroups.x, gl_NumWorkGroups.y * gl_NumWorkGroups.x)));

struct AtomStruct 
{
	vec4 position_radius;
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
uniform uvec3 cell_nr;
uniform vec3 min;
uniform vec3 max;

//function prototypes

uint calculateGirdIndex(vec3 pos);
uvec3 normalize(vec3 pos);
void constructGrid();

//Calculate grid index from position 
uint calculateGridIndex(vec3 pos)
{
	uvec3 u_pos = normalize(pos);
	return cell_nr.x * u_pos.x
		+ cell_nr.x * (cell_nr.y * u_pos.y)
		+ cell_nr.x * cell_nr.y * (u_pos.z * cell_nr.z);
}

uvec3 normalize(vec3 pos) 
{
	return uvec3((pos - min) / (max - min));
}

void constructGrid()
{
	if (id >= nrAtoms) return;
	AtomStruct atom = atoms[id];
	uint gridId = calculateGridIndex(atom.position_radius.xyz);
	GridCell cell = cells[gridId];
	uint id_pos = atomicAdd(cells[gridId].count,1);
	if (id_pos >= MAX_ATOMS_PER_GRID) return;
	cells[gridId].ids[id_pos] = id;
}

void main()
{
	constructGrid();
}
