#include "SESSurface.h"
#include <algorithm>

const int SESSurface::MAX_ATOMS = 1000000;


SESSurface::SESSurface(const Molecule& molecule, float probeRadius)
	:probeRadius(probeRadius),maxRadius(molecule.max_radius), maxPos(molecule.max_pos), minPos(molecule.min_pos), nrOfAtoms((int)molecule.atoms.size())
{
	//Setup Grids
	setupGrids(molecule);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);
	// Build Atom Grind
	//glm::uvec3 dimensions = glm::uvec3((molecule.max_pos - molecule.min_pos + glm::vec3(molecule.max_radius+probeRadius)) / (molecule.max_radius + probeRadius));
	atomSSBO.uploadData(molecule.atoms);
	atomSSBO.bindToTarget(0);
	atomGrid->bindToTarget(1);
	gridBuilder.use();
	gridBuilder.setUniform("nrAtoms", (int)molecule.atoms.size());
	gridBuilder.setUniform("nr_cells", nrCells);
	gridBuilder.setUniform("max", grid_max);
	gridBuilder.setUniform("min", grid_min);
	int number = (int)(molecule.atoms.size() / (1024)) + 1;
	glDispatchCompute(number, 1, 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);

	//Probe Intersection
	glm::uvec3 dispatchSize = glm::uvec3(nrSESVoxel / glm::uvec3(8)) + glm::uvec3(1);
	probeIntersection.use();
	probeIntersection.setUniform("grid_min", grid_min);
	probeIntersection.setUniform("grid_max", grid_max);
	probeIntersection.setUniform("tex_min", tex_min);
	probeIntersection.setUniform("tex_max", tex_max);
	probeIntersection.setUniform("nr_cells", nrCells);
	probeIntersection.setUniform("probeRadius", probeRadius);
	probeIntersection.setUniform("texRadius", this->texResolution);
	SESTexture->bindAsImage(0);
	voxelClassification->bindAsImage(1);
	glDispatchCompute(dispatchSize.x, dispatchSize.y, dispatchSize.z);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	// Distance Field refinement
	dfRefinement.use();
	dfRefinement.setUniform("probeRadius", probeRadius);
	dfRefinement.setUniform("texRadius", this->texResolution);
	glDispatchCompute(dispatchSize.x, dispatchSize.y, dispatchSize.z);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	// Propagate Distance Field
	dfInner.use();
	dfInner.setUniform("texRadius", this->texResolution);
	dispatchSize = nrSESVoxel / glm::uvec3(3) + glm::uvec3(1);

	for (int i = 0; i < 30; ++i)
	{
		glDispatchCompute(dispatchSize.x, dispatchSize.y, dispatchSize.z);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	}
}

float SESSurface::calculateIdealResolution(const Molecule & molecule)
{
	glm::vec3 moleculeDimension = molecule.max_pos - molecule.min_pos + 2.0f*glm::vec3(molecule.max_radius+this->probeRadius);
	float maxDimension = std::max(moleculeDimension.x, std::max(moleculeDimension.y, moleculeDimension.z));
	return std::max(0.4f,std::min(maxDimension / 250.f, 1.0f));
}

void SESSurface::setupGrids(const Molecule & molecule)
{
	float idealRadius = calculateIdealResolution(molecule);
	glm::vec3 moleculeDimension = molecule.max_pos - molecule.min_pos + 2.f*glm::vec3(molecule.max_radius + this->probeRadius);
	tex_min = grid_min = molecule.min_pos - glm::vec3(molecule.max_radius + this->probeRadius);

	// Calculate Atom Grid Dimensions
	nrCells = glm::uvec3(moleculeDimension / (molecule.max_radius + this->probeRadius)) + glm::uvec3(1);
	grid_max = grid_min + glm::vec3(nrCells) * (molecule.max_radius + this->probeRadius);
	atomGrid = new SSBO<GridCell>((nrCells.x+1)*(nrCells.y+1)*(nrCells.z+1));

	// Calculate SESTexture Dimension
	nrSESVoxel = glm::uvec3(moleculeDimension / idealRadius) + glm::uvec3(1);
	tex_max = tex_min + glm::vec3(nrSESVoxel)*glm::vec3(idealRadius);
	SESTexture = new VolumetricTexture(nrSESVoxel.x, nrSESVoxel.y, nrSESVoxel.z, GL_R32F, GL_RED, GL_FLOAT);
	voxelClassification = new VolumetricTexture(nrSESVoxel.x, nrSESVoxel.y, nrSESVoxel.z, GL_R16F, GL_RED, GL_FLOAT);
	texResolution = idealRadius;
}

void SESSurface::recalculateGrid()
{
	glm::vec3 moleculeDimension = maxPos - minPos + 2.f*glm::vec3(maxRadius + this->probeRadius);
	glm::vec3 resolutions = (moleculeDimension)/glm::vec3(nrSESVoxel);
	this->texResolution = std::max(resolutions.x, std::max(resolutions.y, resolutions.z));
	tex_min = grid_min = minPos - glm::vec3(maxRadius + this->probeRadius);

	//AtomGrid
	nrCells = glm::uvec3(moleculeDimension / (maxRadius + this->probeRadius)) + glm::uvec3(1);
	delete atomGrid;
	atomGrid = new SSBO<GridCell>((nrCells.x + 1)*(nrCells.y + 1)*(nrCells.z + 1));
	grid_max = grid_min + glm::vec3(nrCells) * (maxRadius + this->probeRadius);

	//Textures
	tex_max = tex_min + glm::vec3(nrSESVoxel)*glm::vec3(texResolution);

}

SESSurface::~SESSurface()
{
	delete atomGrid;
	delete SESTexture;
	delete voxelClassification;
}

void SESSurface::bindToUnit(int unit)
{
	SESTexture->bindAsTexture(unit);
}

glm::vec3 SESSurface::getTexMin()
{
	return tex_min;
}

glm::vec3 SESSurface::getTexMax()
{
	return tex_max;
}

void SESSurface::updateRadius(float probeRadius)
{
	this->probeRadius = probeRadius;
	recalculateGrid();
	glMemoryBarrier(GL_ALL_BARRIER_BITS);
	//Calculations
	atomSSBO.bindToTarget(0);
	atomGrid->bindToTarget(1);
	gridBuilder.use();
	gridBuilder.setUniform("nrAtoms", nrOfAtoms);
	gridBuilder.setUniform("nr_cells", nrCells);
	gridBuilder.setUniform("max", grid_max);
	gridBuilder.setUniform("min", grid_min);
	int number = (int)((nrOfAtoms) / (1024)) + 1;
	glDispatchCompute(number, 1, 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);

	//Probe Intersection
	glm::uvec3 dispatchSize = glm::uvec3(nrSESVoxel / glm::uvec3(8)) + glm::uvec3(1);
	probeIntersection.use();
	probeIntersection.setUniform("grid_min", grid_min);
	probeIntersection.setUniform("grid_max", grid_max);
	probeIntersection.setUniform("tex_min", tex_min);
	probeIntersection.setUniform("tex_max", tex_max);
	probeIntersection.setUniform("nr_cells", nrCells);
	probeIntersection.setUniform("probeRadius", probeRadius);
	probeIntersection.setUniform("texRadius", this->texResolution);
	SESTexture->bindAsImage(0);
	voxelClassification->bindAsImage(1);
	glDispatchCompute(dispatchSize.x, dispatchSize.y, dispatchSize.z);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	// Distance Field refinement
	dfRefinement.use();
	dfRefinement.setUniform("probeRadius", probeRadius);
	dfRefinement.setUniform("texRadius", this->texResolution);
	glDispatchCompute(dispatchSize.x, dispatchSize.y, dispatchSize.z);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	// Propagate Distance Field
	dfInner.use();
	dfInner.setUniform("texRadius", this->texResolution);
	dispatchSize = nrSESVoxel / glm::uvec3(3) + glm::uvec3(1);

	for (int i = 0; i < 30; ++i)
	{
		glDispatchCompute(dispatchSize.x, dispatchSize.y, dispatchSize.z);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	}
}
