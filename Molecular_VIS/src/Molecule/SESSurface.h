#pragma once
#include <memory>

#include <glm/glm.hpp>

#include "Molecule.h"
#include "../Texture/VolumetricTexture.h"
#include "../Shader/Shader.h"
#include "../Shader/SSBO.h"

/*!
 * \brief Struct for the cells of the grid where the atoms are stored in. This grid is used for faster neighbor lookups.
 * 
 */
struct GridCell
{
	/*! Number of Atoms in this cell*/
	unsigned int count;

	/*! First 31 Atoms in this cell*/
	unsigned int ids[31];
};

/*!
 * \brief Class for the surface representation of a Molecule. 
 * 
 * The surface is defined as the solvent excluded surface (SES) and is represented by a signed distance field.
 * 
 */
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
	/*!
	 * Constructs SES of a given molecule and probe radius.
	 * 
	 * \param molecule Data of molecule.
	 * \param probeRadius probe radius used to define the SES.
	 */
	SESSurface(const Molecule& molecule, float probeRadius);
	~SESSurface();

	/*!
	 * Binds the signed distance field as a texture to the given texture-unit at target GL_TEXTURE_3D.
	 * 
	 * \param unit Texture-unit to bind to.
	 */
	void bindToUnit(int unit);

	/*!
	 * Gets minimal bounding box corner.
	 * 
	 * \return minimal bounding box corner. All atoms of molecule have all components larger then this value. 
	 */
	glm::vec3 getTexMin();

	/*!
	 * Gets maximal bounding box corner.
	 *
	 * \return maximal bounding box corner. All atoms of molecule have all components smaller then this value.
	 */
	glm::vec3 getTexMax();
};