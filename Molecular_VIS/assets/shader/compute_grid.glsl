#version 430 core

struct AtomStruct 
{
	vec4 position_radius;
	int id;
};

layout(std430, binding = 0) buffer readonly Atoms
{
	AtomStruct atoms[];
};

layout(std430, binding = 1) buffer Grid
{
	int gridCount[]
};

uniform int nrAtoms;

// Size of grid, i.e how many cells per dimension
uniform uint res_width;
uniform uint res_height;
uniform uint res_depth;

// Dimension of Grid(physical)
uniform uint width;
uniform uint height;
uniform uint depth;


//Calculate grid index from position 
int calculateGridIndex(float x; float y; float z)
{
	int = res_width * x / width + res_width * (res_height * y / height) +  res_width * res_height * (z * res_depth / depth);
}