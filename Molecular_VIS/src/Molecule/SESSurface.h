#pragma once
#include <memory>

#include <glm/glm.hpp>

#include "Molecule.h"
#include "../Texture/VolumetricTexture.h"
#include "../Shader/Shader.h"
#include "../Shader/SSBO.h"

struct GridCell
{
	unsigned int count;
	unsigned int ids[31];
};

class SESSurface
{
private:
	//Compute Shaders
	const static int MAX_ATOMS;
	Shader gridBuilder = Shader("compute_grid.glsl");
	Shader probeIntersection = Shader("probe_intersection.glsl");
	Shader dfRefinement = Shader("df_refinement.glsl");
	Shader dfInner = Shader("df_inner.glsl");
	SSBO<Atom> atomSSBO = SSBO<Atom>(MAX_ATOMS);

	float probeRadius;
	float texResolution;
	// Volumetric Texture
	glm::vec3 tex_min;
	glm::vec3 tex_max;
	glm::uvec3 nrSESVoxel;
	VolumetricTexture* SESTexture;
	VolumetricTexture* voxelClassification;

	// Atom Grid
	glm::vec3 grid_min;
	glm::vec3 grid_max;
	glm::uvec3 nrCells;
	SSBO<GridCell>* atomGrid;

	float calculateIdealResolution(const Molecule& molecule);
	void setupGrids(const Molecule& molecule);

	
public:
	SESSurface(const Molecule& molecule, float probeRadius);
	~SESSurface();

	void bindToUnit(int unit);
	glm::vec3 getTexMin();
	glm::vec3 getTexMax();
};